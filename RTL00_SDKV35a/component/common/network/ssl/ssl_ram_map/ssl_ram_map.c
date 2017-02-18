#include "rom_ssl_ram_map.h"
#include <diag.h>
#include <polarssl/ssl.h>

extern struct _rom_ssl_ram_map rom_ssl_ram_map;

//AES HW CRYPTO
extern int rtl_crypto_aes_ecb_init(IN const u8* key, IN const u32 keylen);
extern int rtl_crypto_aes_ecb_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_aes_ecb_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_aes_cbc_init(IN const u8* key, 	IN const u32 keylen);
extern int rtl_crypto_aes_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_aes_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);

//DES HW CRYPTO
extern int rtl_crypto_des_cbc_init(IN const u8* key, IN const u32 keylen);
extern int rtl_crypto_des_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_des_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_3des_cbc_init(IN const u8* key, IN const u32 keylen);
extern int rtl_crypto_3des_cbc_decrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern int rtl_crypto_3des_cbc_encrypt(
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* iv, 		IN const u32 ivlen, 
	OUT u8* pResult);

extern int ssl_max_frag_len;

int platform_set_malloc_free( void * (*malloc_func)( size_t ),
                              void (*free_func)( void * ) )
{
	/* OS interface */
	rom_ssl_ram_map.ssl_malloc = malloc_func;
	rom_ssl_ram_map.ssl_free = free_func;
	rom_ssl_ram_map.ssl_printf = (int (*)(char const *, ...))DiagPrintf;

	//AES HW CRYPTO
	rom_ssl_ram_map.hw_crypto_aes_ecb_init = rtl_crypto_aes_ecb_init;
	rom_ssl_ram_map.hw_crypto_aes_ecb_decrypt = rtl_crypto_aes_ecb_decrypt;
	rom_ssl_ram_map.hw_crypto_aes_ecb_encrypt = rtl_crypto_aes_ecb_encrypt;
	rom_ssl_ram_map.hw_crypto_aes_cbc_init = rtl_crypto_aes_cbc_init;
	rom_ssl_ram_map.hw_crypto_aes_cbc_decrypt = rtl_crypto_aes_cbc_decrypt;
	rom_ssl_ram_map.hw_crypto_aes_cbc_encrypt = rtl_crypto_aes_cbc_encrypt;

	//DES HW CRYPTO
	rom_ssl_ram_map.hw_crypto_des_cbc_init = rtl_crypto_des_cbc_init;
	rom_ssl_ram_map.hw_crypto_des_cbc_decrypt = rtl_crypto_des_cbc_decrypt;
	rom_ssl_ram_map.hw_crypto_des_cbc_encrypt = rtl_crypto_des_cbc_encrypt;
	rom_ssl_ram_map.hw_crypto_3des_cbc_init = rtl_crypto_3des_cbc_init;
	rom_ssl_ram_map.hw_crypto_3des_cbc_decrypt = rtl_crypto_3des_cbc_decrypt;
	rom_ssl_ram_map.hw_crypto_3des_cbc_encrypt = rtl_crypto_3des_cbc_encrypt;

	/* Variables */
	rom_ssl_ram_map.use_hw_crypto_func = 1;
	
	int len = ssl_max_frag_len;
			if(len == 0) len = 8192;
			else if(len < 512) len = 512;
			else if(len > 16384) len = 16384;
	rom_ssl_ram_map.ssl_buffer_len = len + SSL_COMPRESSION_ADD
			+ 29 /* counter + header + IV */
			+ SSL_MAC_ADD
			+ SSL_PADDING_ADD;
#if defined(POLARSSL_SSL_MAX_FRAGMENT_LENGTH)
	 mfl_code_to_length[0] = len;
#endif
	return 0;
}
