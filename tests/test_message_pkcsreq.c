#include <check.h>
#include "scep.h"
#include "test_message_common.c"

static PKCS7 *make_message(X509_REQ *req)
{
    PKCS7 *p7 = NULL;
    SCEP_ERROR err = scep_pkcsreq(
        handle, req, sig_cert, sig_key, enc_cacert, &p7);
    ck_assert(err == SCEPE_OK);
    ck_assert(p7 != NULL);
    return p7;
}

static free_message(PKCS7 *p7)
{
    if(p7)
        PKCS7_free(p7);
}

static void setup()
{
    generic_setup();

    p7 = make_message(req);
    scep_conf_set(handle, SCEPCFG_FLAG_SET, SCEP_SKIP_SIGNER_CERT);
    p7_nosigcert = make_message(req);
}

static void teardown()
{
    free_message(p7);
    p7 = NULL;
    free_message(p7_nosigcert);
    p7_nosigcert = NULL;
    generic_teardown();
}

START_TEST(test_pkcsreq)
{
    BIO *data = get_decrypted_data(p7, enc_cacert, enc_cakey);
    X509_REQ *csr = d2i_X509_REQ_bio(data, NULL);
    ck_assert(csr != NULL);
    BIO_free(data);

    data = BIO_new(BIO_s_mem());
    BIO_puts(data, test_new_csr);
    X509_REQ *ref_csr = PEM_read_bio_X509_REQ(data, NULL, 0, 0);
    BIO_free(data);

    ck_assert_int_eq(X509_REQ_cmp(csr, ref_csr), 0);
    ck_assert_str_eq(
        SCEP_MSG_PKCSREQ_STR,
        get_attribute_data(p7, handle->oids->messageType));
    X509_REQ_free(ref_csr);
    X509_REQ_free(csr);
    free_message(p7);
}
END_TEST

START_TEST(test_missing_dn)
{
    BIGNUM *bne = BN_new();
    ck_assert_int_eq(BN_set_word(bne, RSA_F4), 1);
    RSA *r = RSA_new();
    ck_assert_int_ne(RSA_generate_key_ex(r, 2048, bne, NULL), 0);
    BN_free(bne);

    X509_REQ *req = X509_REQ_new();
    ck_assert_int_ne(X509_REQ_set_version(req, 1), 0);

    EVP_PKEY *key = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(key, r);
    ck_assert_int_ne(X509_REQ_set_pubkey(req, key), 0);
    ck_assert_int_ne(X509_REQ_sign(req, key, EVP_sha1()), 0);
    EVP_PKEY_free(key);

    X509 *sig_cert = NULL, *enc_cert = NULL;
    EVP_PKEY *sig_key = NULL;
    p7 = NULL;
    ck_assert(scep_pkcsreq(handle, req, sig_cert, sig_key, enc_cert, &p7) == SCEPE_INVALID_CONTENT);
    ck_assert(p7 == NULL);
    X509_REQ_free(req);
}
END_TEST

START_TEST(test_missing_pubkey)
{
    BIGNUM *bne = BN_new();
    ck_assert_int_eq(BN_set_word(bne, RSA_F4), 1);
    RSA *r = RSA_new();
    ck_assert_int_ne(RSA_generate_key_ex(r, 2048, bne, NULL), 0);
    BN_free(bne);

    X509_REQ *req = X509_REQ_new();
    ck_assert_int_ne(X509_REQ_set_version(req, 1), 0);

    X509_NAME *name = X509_REQ_get_subject_name(req);
    ck_assert_int_ne(X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, "DE", -1, -1, 0), 0);

    EVP_PKEY *key = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(key, r);
    ck_assert_int_ne(X509_REQ_sign(req, key, EVP_sha1()), 0);
    EVP_PKEY_free(key);

    X509 *sig_cert = NULL, *enc_cert = NULL;
    EVP_PKEY *sig_key = NULL;
    p7 = NULL;
    ck_assert(scep_pkcsreq(handle, req, sig_cert, sig_key, enc_cert, &p7) == SCEPE_INVALID_CONTENT);
    ck_assert(p7 == NULL);
    X509_REQ_free(req);
}
END_TEST

START_TEST(test_missing_challenge_password)
{
    BIGNUM *bne = BN_new();
    ck_assert_int_eq(BN_set_word(bne, RSA_F4), 1);
    RSA *r = RSA_new();
    ck_assert_int_ne(RSA_generate_key_ex(r, 2048, bne, NULL), 0);
    BN_free(bne);

    X509_REQ *req = X509_REQ_new();
    ck_assert_int_ne(X509_REQ_set_version(req, 1), 0);

    X509_NAME *name = X509_REQ_get_subject_name(req);
    ck_assert_int_ne(X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, "DE", -1, -1, 0), 0);

    EVP_PKEY *key = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(key, r);
    ck_assert_int_ne(X509_REQ_set_pubkey(req, key), 0);
    ck_assert_int_ne(X509_REQ_sign(req, key, EVP_sha1()), 0);
    EVP_PKEY_free(key);

    X509 *sig_cert = NULL, *enc_cert = NULL;
    EVP_PKEY *sig_key = NULL;
    p7 = NULL;
    ck_assert(scep_pkcsreq(handle, req, sig_cert, sig_key, enc_cert, &p7) == SCEPE_INVALID_CONTENT);
    ck_assert(p7 == NULL);
    X509_REQ_free(req);
}
END_TEST

void add_pkcsreq(Suite *s)
{
    TCase *tc_pkcsreq_msg = tcase_create("PKCSReq Message");
    tcase_add_unchecked_fixture(tc_pkcsreq_msg, setup, teardown);
    tcase_add_test(tc_pkcsreq_msg, test_scep_message_asn1_version);
    tcase_add_test(tc_pkcsreq_msg, test_scep_message_transaction_id);
    tcase_add_test(tc_pkcsreq_msg, test_scep_message_sender_nonce);
    tcase_add_test(tc_pkcsreq_msg, test_scep_message_type);
    tcase_add_test(tc_pkcsreq_msg, test_scep_message_content_type);
    tcase_add_test(tc_pkcsreq_msg, test_scep_message_certificate);
    tcase_add_test(tc_pkcsreq_msg, test_pkcsreq);
    suite_add_tcase(s, tc_pkcsreq_msg);

    TCase *tc_pkcsreq_errors = tcase_create("PKCSReq Invalid");
    tcase_add_unchecked_fixture(tc_pkcsreq_errors, generic_setup, generic_teardown);
    tcase_add_test(tc_pkcsreq_errors, test_missing_dn);
    tcase_add_test(tc_pkcsreq_errors, test_missing_pubkey);
    tcase_add_test(tc_pkcsreq_errors, test_missing_challenge_password);
    suite_add_tcase(s, tc_pkcsreq_errors);

    TCase *tc_unwrap = tcase_create("PKCSReq Unwrapping");
    tcase_add_unchecked_fixture(tc_unwrap, setup, teardown);
    suite_add_tcase(s, tc_unwrap);
}