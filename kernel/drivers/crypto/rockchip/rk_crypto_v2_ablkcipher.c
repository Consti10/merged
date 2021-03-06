// SPDX-License-Identifier: GPL-2.0
/*
 * Crypto acceleration support for Rockchip Crypto V2
 *
 * Copyright (c) 2018, Fuzhou Rockchip Electronics Co., Ltd
 *
 * Author: Lin Jinhan <troy.lin@rock-chips.com>
 *
 * Some ideas are from marvell-cesa.c and s5p-sss.c driver.
 */

#include <linux/module.h>
#include <linux/platform_device.h>

#include "rk_crypto_core.h"
#include "rk_crypto_v2.h"
#include "rk_crypto_v2_reg.h"

#define MASK_BC_MODE(mode)	((mode) & 0x00f0)
#define IS_BC_DECRYPT(mode)	(!!((mode) & CRYPTO_BC_DECRYPT))

static const u32 cipher_algo2bc[] = {
	[CIPHER_ALGO_DES]      = CRYPTO_BC_DES,
	[CIPHER_ALGO_DES3_EDE] = CRYPTO_BC_TDES,
	[CIPHER_ALGO_AES]      = CRYPTO_BC_AES,
	[CIPHER_ALGO_SM4]      = CRYPTO_BC_SM4,
};

static const u32 cipher_mode2bc[] = {
	[CIPHER_MODE_ECB] = CRYPTO_BC_ECB,
	[CIPHER_MODE_CBC] = CRYPTO_BC_CBC,
	[CIPHER_MODE_CFB] = CRYPTO_BC_CFB,
	[CIPHER_MODE_OFB] = CRYPTO_BC_OFB,
	[CIPHER_MODE_CTR] = CRYPTO_BC_CTR,
	[CIPHER_MODE_XTS] = CRYPTO_BC_XTS,
};

static int rk_crypto_irq_handle(int irq, void *dev_id)
{
	struct rk_crypto_info *dev = platform_get_drvdata(dev_id);
	u32 interrupt_status;
	struct rk_hw_crypto_v2_info *hw_info =
			(struct rk_hw_crypto_v2_info *)dev->hw_info;

	interrupt_status = CRYPTO_READ(dev, CRYPTO_DMA_INT_ST);
	CRYPTO_WRITE(dev, CRYPTO_DMA_INT_ST, interrupt_status);

	if (interrupt_status != CRYPTO_DST_ITEM_DONE_INT_ST) {
		dev_err(dev->dev, "DMA desc = %p\n", hw_info->desc);
		dev_err(dev->dev, "DMA addr_in = %08x\n",
			(u32)dev->addr_in);
		dev_err(dev->dev, "DMA addr_out = %08x\n",
			(u32)dev->addr_out);
		dev_err(dev->dev, "DMA count = %08x\n", dev->count);
		dev_err(dev->dev, "DMA desc_dma = %08x\n",
			(u32)hw_info->desc_dma);
		dev_err(dev->dev, "DMA Error status = %08x\n",
			interrupt_status);
		dev_err(dev->dev, "DMA CRYPTO_DMA_LLI_ADDR status = %08x\n",
			CRYPTO_READ(dev, CRYPTO_DMA_LLI_ADDR));
		dev_err(dev->dev, "DMA CRYPTO_DMA_ST status = %08x\n",
			CRYPTO_READ(dev, CRYPTO_DMA_ST));
		dev_err(dev->dev, "DMA CRYPTO_DMA_STATE status = %08x\n",
			CRYPTO_READ(dev, CRYPTO_DMA_STATE));
		dev_err(dev->dev, "DMA CRYPTO_DMA_LLI_RADDR status = %08x\n",
			CRYPTO_READ(dev, CRYPTO_DMA_LLI_RADDR));
		dev_err(dev->dev, "DMA CRYPTO_DMA_SRC_RADDR status = %08x\n",
			CRYPTO_READ(dev, CRYPTO_DMA_SRC_RADDR));
		dev_err(dev->dev, "DMA CRYPTO_DMA_DST_RADDR status = %08x\n",
			CRYPTO_READ(dev, CRYPTO_DMA_DST_RADDR));
		dev->err = -EFAULT;
	}

	return 0;
}

static u32 byte2word(const u8 *ch, u32 endian)
{
	u32 w = 0;

	/* 0: Big-Endian 1: Little-Endian */
	if (endian == BIG_ENDIAN)
		w = (*ch << 24) + (*(ch + 1) << 16) +
		    (*(ch + 2) << 8) + *(ch + 3);
	else if (endian == LITTLE_ENDIAN)
		w = (*(ch + 3) << 24) + (*(ch + 2) << 16) +
		    (*(ch + 1) << 8) + *ch;

	return w;
}

static void set_iv_reg(struct rk_crypto_info *dev, const u8 *iv, u32 iv_len)
{
	u32 i;
	u8 tmp_buf[4];
	u32 base_iv;

	base_iv = CRYPTO_CH0_IV_0;
	/* write iv data to reg */
	for (i = 0; i < iv_len / 4; i++, base_iv += 4)
		CRYPTO_WRITE(dev, base_iv, byte2word(iv + i * 4, BIG_ENDIAN));

	if (iv_len % 4) {
		memset(tmp_buf, 0x00, sizeof(tmp_buf));
		memcpy((u8 *)tmp_buf, iv + (iv_len / 4) * 4, iv_len % 4);
		CRYPTO_WRITE(dev, base_iv, byte2word(tmp_buf, BIG_ENDIAN));
	}

	CRYPTO_WRITE(dev, CRYPTO_CH0_IV_LEN_0, iv_len);
}

static void write_key_reg(struct rk_crypto_info *dev, const u8 *key,
			  u32 key_len)
{
	u32 i;
	u8 tmp_buf[4];
	u32 chn_base_addr;

	chn_base_addr = CRYPTO_CH0_KEY_0;

	for (i = 0; i < key_len / 4; i++, chn_base_addr += 4)
		CRYPTO_WRITE(dev, chn_base_addr,
			     byte2word(key + i * 4, BIG_ENDIAN));

	if (key_len % 4) {
		memset(tmp_buf, 0x00, sizeof(tmp_buf));
		memcpy((u8 *)tmp_buf, key + i * 4, key_len % 4);
		CRYPTO_WRITE(dev, chn_base_addr,
			     byte2word(tmp_buf, BIG_ENDIAN));
	}
}

static void write_tkey_reg(struct rk_crypto_info *dev, const u8 *key,
			   u32 key_len)
{
	u32 i;
	u8 tmp_buf[4];
	u32 chn_base_addr;

	chn_base_addr = CRYPTO_CH4_KEY_0;

	for (i = 0; i < key_len / 4; i++, chn_base_addr += 4)
		CRYPTO_WRITE(dev, chn_base_addr,
			     byte2word(key + i * 4, BIG_ENDIAN));

	if (key_len % 4) {
		memset(tmp_buf, 0x00, sizeof(tmp_buf));
		memcpy((u8 *)tmp_buf, key + i * 4, key_len % 4);
		CRYPTO_WRITE(dev, chn_base_addr,
			     byte2word(tmp_buf, BIG_ENDIAN));
	}
}

static struct rk_crypto_tmp *rk_cipher_get_algt(struct crypto_ablkcipher *tfm)
{
	struct crypto_alg *alg = tfm->base.__crt_alg;

	return container_of(alg, struct rk_crypto_tmp, alg.crypto);
}

static bool is_use_fallback(struct rk_cipher_ctx *ctx)
{
	return ctx->keylen == AES_KEYSIZE_192 && ctx->fallback_tfm;
}

static bool is_no_multi_blocksize(struct rk_crypto_info *dev)
{
	struct ablkcipher_request *req =
		ablkcipher_request_cast(dev->async_req);
	struct crypto_ablkcipher *cipher = crypto_ablkcipher_reqtfm(req);
	struct rk_crypto_tmp *algt = rk_cipher_get_algt(cipher);

	return (algt->mode == CIPHER_MODE_CFB ||
			algt->mode == CIPHER_MODE_OFB ||
			algt->mode == CIPHER_MODE_CTR) ? true : false;
}

static void rk_crypto_complete(struct crypto_async_request *base, int err)
{
	if (base->complete)
		base->complete(base, err);
}

static int rk_handle_req(struct rk_crypto_info *dev,
			 struct ablkcipher_request *req)
{
	if (!IS_ALIGNED(req->nbytes, dev->align_size) &&
	    !is_no_multi_blocksize(dev))
		return -EINVAL;
	else
		return dev->enqueue(dev, &req->base);
}

static int rk_cipher_setkey(struct crypto_ablkcipher *cipher,
			    const u8 *key, unsigned int keylen)
{
	struct crypto_tfm *tfm = crypto_ablkcipher_tfm(cipher);
	struct rk_crypto_tmp *algt = rk_cipher_get_algt(cipher);
	struct rk_cipher_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 tmp[DES_EXPKEY_WORDS];
	int ret = -EINVAL;

	CRYPTO_MSG("algo = %x, mode = %x, key_len = %d\n",
		   algt->algo, algt->mode, keylen);

	switch (algt->algo) {
	case CIPHER_ALGO_DES:
		if (keylen != DES_KEY_SIZE)
			goto error;

		if (!des_ekey(tmp, key) &&
		    (tfm->crt_flags & CRYPTO_TFM_REQ_WEAK_KEY)) {
			tfm->crt_flags |= CRYPTO_TFM_RES_WEAK_KEY;
			return -EINVAL;
		}
		break;
	case CIPHER_ALGO_DES3_EDE:
		if (keylen != DES3_EDE_KEY_SIZE)
			goto error;
		break;
	case CIPHER_ALGO_AES:
		if (algt->mode != CIPHER_MODE_XTS) {
			if (keylen != AES_KEYSIZE_128 &&
			    keylen != AES_KEYSIZE_192 &&
			    keylen != AES_KEYSIZE_256)
				goto error;
		} else {
			if (keylen != AES_KEYSIZE_256 &&
			    keylen != AES_KEYSIZE_256 * 2)
				goto error;
		}
		break;
	case CIPHER_ALGO_SM4:
		if (algt->mode != CIPHER_MODE_XTS) {
			if (keylen != SM4_KEY_SIZE)
				goto error;
		} else {
			if (keylen != SM4_KEY_SIZE * 2)
				goto error;
		}
		break;
	default:
		goto error;
	}

	memcpy(ctx->key, key, keylen);
	ctx->keylen = keylen;

	if (algt->mode == CIPHER_MODE_XTS)
		ctx->keylen /= 2;

	if (is_use_fallback(ctx)) {
		CRYPTO_MSG("use fallback tfm");
		ret = crypto_skcipher_setkey(ctx->fallback_tfm, key, keylen);
		if (ret) {
			CRYPTO_MSG("soft fallback crypto_skcipher_setkey err = %d\n", ret);
			goto error;
		}
	}

	return 0;

error:
	crypto_ablkcipher_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_LEN);
	return ret;
}

static int rk_cipher_encrypt(struct ablkcipher_request *req)
{
	struct crypto_ablkcipher *tfm = crypto_ablkcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_ablkcipher_ctx(tfm);
	struct rk_crypto_tmp *algt = rk_cipher_get_algt(tfm);
	int ret = -EINVAL;

	CRYPTO_TRACE();

	if (is_use_fallback(ctx)) {
		SKCIPHER_REQUEST_ON_STACK(subreq, ctx->fallback_tfm);

		CRYPTO_MSG("use fallback tfm");

		skcipher_request_set_tfm(subreq, ctx->fallback_tfm);
		skcipher_request_set_callback(subreq, req->base.flags,
					      NULL, NULL);
		skcipher_request_set_crypt(subreq, req->src, req->dst,
					   req->nbytes, req->info);
		ret =  crypto_skcipher_encrypt(subreq);
		skcipher_request_zero(subreq);
	} else {
		ctx->mode = cipher_algo2bc[algt->algo] | cipher_mode2bc[algt->mode];
		CRYPTO_MSG("ctx->mode = %x\n", ctx->mode);
		ret = rk_handle_req(ctx->dev, req);
	}

	return ret;
}

static int rk_cipher_decrypt(struct ablkcipher_request *req)
{
	struct crypto_ablkcipher *tfm = crypto_ablkcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_ablkcipher_ctx(tfm);
	struct rk_crypto_tmp *algt = rk_cipher_get_algt(tfm);
	int ret = -EINVAL;

	CRYPTO_TRACE();

	if (is_use_fallback(ctx)) {
		SKCIPHER_REQUEST_ON_STACK(subreq, ctx->fallback_tfm);

		CRYPTO_MSG("use soft fallback tfm");

		skcipher_request_set_tfm(subreq, ctx->fallback_tfm);
		skcipher_request_set_callback(subreq, req->base.flags,
					      NULL, NULL);
		skcipher_request_set_crypt(subreq, req->src, req->dst,
					   req->nbytes, req->info);
		ret =  crypto_skcipher_decrypt(subreq);
		skcipher_request_zero(subreq);
	} else {
		ctx->mode = cipher_algo2bc[algt->algo] |
			    cipher_mode2bc[algt->mode] |
			    CRYPTO_BC_DECRYPT;
		CRYPTO_MSG("ctx->mode = %x\n", ctx->mode);
		ret = rk_handle_req(ctx->dev, req);
	}

	return ret;
}

static void rk_ablk_hw_init(struct rk_crypto_info *dev)
{
	struct ablkcipher_request *req =
		ablkcipher_request_cast(dev->async_req);
	struct crypto_ablkcipher *cipher = crypto_ablkcipher_reqtfm(req);
	struct crypto_tfm *tfm = crypto_ablkcipher_tfm(cipher);
	struct rk_cipher_ctx *ctx = crypto_ablkcipher_ctx(cipher);
	u32 ivsize, block;

	CRYPTO_WRITE(dev, CRYPTO_BC_CTL, 0x00010000);

	block = crypto_tfm_alg_blocksize(tfm);
	ivsize = crypto_ablkcipher_ivsize(cipher);

	write_key_reg(ctx->dev, ctx->key, ctx->keylen);
	if (MASK_BC_MODE(ctx->mode) == CRYPTO_BC_XTS)
		write_tkey_reg(ctx->dev, ctx->key + ctx->keylen, ctx->keylen);

	if (MASK_BC_MODE(ctx->mode) != CRYPTO_BC_ECB)
		set_iv_reg(dev, req->info, ivsize);

	if (block != DES_BLOCK_SIZE) {
		if (ctx->keylen == AES_KEYSIZE_128)
			ctx->mode |= CRYPTO_BC_128_bit_key;
		else if (ctx->keylen == AES_KEYSIZE_192)
			ctx->mode |= CRYPTO_BC_192_bit_key;
		else if (ctx->keylen == AES_KEYSIZE_256)
			ctx->mode |= CRYPTO_BC_256_bit_key;
	}

	ctx->mode |= CRYPTO_BC_ENABLE;

	CRYPTO_WRITE(dev, CRYPTO_FIFO_CTL, 0x00030003);

	CRYPTO_WRITE(dev, CRYPTO_DMA_INT_EN, 0x7f);

	CRYPTO_WRITE(dev, CRYPTO_BC_CTL, ctx->mode | CRYPTO_WRITE_MASK_ALL);
}

static void crypto_dma_start(struct rk_crypto_info *dev)
{
	struct rk_hw_crypto_v2_info *hw_info =
			(struct rk_hw_crypto_v2_info *)dev->hw_info;
	u32 calc_len = dev->count;

	memset(hw_info->desc, 0x00, sizeof(*hw_info->desc));

	/*
	 *	the data length is not aligned will use addr_vir to calculate,
	 *	so crypto v2 could round up date date length to align_size
	 */
	if (is_no_multi_blocksize(dev))
		calc_len = round_up(calc_len, dev->align_size);

	hw_info->desc->src_addr = dev->addr_in;
	hw_info->desc->src_len  = calc_len;
	hw_info->desc->dst_addr = dev->addr_out;
	hw_info->desc->dst_len  = calc_len;
	hw_info->desc->next_addr = 0;
	hw_info->desc->dma_ctrl = 0x00000201;
	hw_info->desc->user_define = 0x7;

	dma_wmb();

	CRYPTO_WRITE(dev, CRYPTO_DMA_LLI_ADDR, hw_info->desc_dma);
	CRYPTO_WRITE(dev, CRYPTO_DMA_CTL, 0x00010001);/* start */
}

static int rk_set_data_start(struct rk_crypto_info *dev)
{
	int err;

	err = dev->load_data(dev, dev->sg_src, dev->sg_dst);
	if (!err)
		crypto_dma_start(dev);
	return err;
}

static int rk_ablk_start(struct rk_crypto_info *dev)
{
	struct ablkcipher_request *req =
		ablkcipher_request_cast(dev->async_req);
	unsigned long flags;
	int err = 0;

	dev->left_bytes = req->nbytes;
	dev->total = req->nbytes;
	dev->sg_src = req->src;
	dev->first = req->src;
	dev->src_nents = sg_nents(req->src);
	dev->sg_dst = req->dst;
	dev->dst_nents = sg_nents(req->dst);
	dev->aligned = 1;

	spin_lock_irqsave(&dev->lock, flags);
	rk_ablk_hw_init(dev);
	err = rk_set_data_start(dev);
	spin_unlock_irqrestore(&dev->lock, flags);
	return err;
}

static void rk_iv_copyback(struct rk_crypto_info *dev)
{
	struct ablkcipher_request *req =
		ablkcipher_request_cast(dev->async_req);
	struct crypto_ablkcipher *tfm = crypto_ablkcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_ablkcipher_ctx(tfm);

	u32 ivsize = crypto_ablkcipher_ivsize(tfm);

	/* Update the IV buffer to contain the next IV for encryption mode. */
	if (!IS_BC_DECRYPT(ctx->mode)) {
		if (dev->aligned) {
			memcpy(req->info, sg_virt(dev->sg_dst) +
				dev->sg_dst->length - ivsize, ivsize);
		} else {
			memcpy(req->info, dev->addr_vir +
				dev->count - ivsize, ivsize);
		}
	}

}

static void rk_update_iv(struct rk_crypto_info *dev)
{
	struct ablkcipher_request *req =
		ablkcipher_request_cast(dev->async_req);
	struct crypto_ablkcipher *tfm = crypto_ablkcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_ablkcipher_ctx(tfm);
	u32 ivsize = crypto_ablkcipher_ivsize(tfm);
	u8 *new_iv = NULL;

	if (IS_BC_DECRYPT(ctx->mode)) {
		new_iv = ctx->iv;
	} else {
		new_iv = page_address(sg_page(dev->sg_dst)) +
			 dev->sg_dst->offset + dev->sg_dst->length - ivsize;
	}

	set_iv_reg(dev, new_iv, ivsize);
}

/* return:
 *	true	some err was occurred
 *	fault	no err, continue
 */
static int rk_ablk_rx(struct rk_crypto_info *dev)
{
	int err = 0;
	struct ablkcipher_request *req =
		ablkcipher_request_cast(dev->async_req);

	dev->unload_data(dev);
	if (!dev->aligned) {
		if (!sg_pcopy_from_buffer(req->dst, dev->dst_nents,
					  dev->addr_vir, dev->count,
					  dev->total - dev->left_bytes -
					  dev->count)) {
			err = -EINVAL;
			goto out_rx;
		}
	}
	if (dev->left_bytes) {
		rk_update_iv(dev);
		if (dev->aligned) {
			if (sg_is_last(dev->sg_src)) {
				dev_err(dev->dev, "[%s:%d] Lack of data\n",
					__func__, __LINE__);
				err = -ENOMEM;
				goto out_rx;
			}
			dev->sg_src = sg_next(dev->sg_src);
			dev->sg_dst = sg_next(dev->sg_dst);
		}
		err = rk_set_data_start(dev);
	} else {
		rk_iv_copyback(dev);
		/* here show the calculation is over without any err */
		dev->complete(dev->async_req, 0);
		tasklet_schedule(&dev->queue_task);
	}
out_rx:
	return err;
}

static int rk_ablk_cra_init(struct crypto_tfm *tfm)
{
	struct rk_crypto_tmp *algt = rk_cipher_get_algt(__crypto_ablkcipher_cast(tfm));
	struct rk_cipher_ctx *ctx = crypto_tfm_ctx(tfm);
	const char *alg_name = crypto_tfm_alg_name(tfm);
	struct rk_crypto_info *info = algt->dev;

	CRYPTO_TRACE();

	if (!info->request_crypto)
		return -EFAULT;

	info->request_crypto(info, alg_name);

	info->align_size = crypto_tfm_alg_blocksize(tfm);
	info->start = rk_ablk_start;
	info->update = rk_ablk_rx;
	info->complete = rk_crypto_complete;
	info->irq_handle = rk_crypto_irq_handle;

	ctx->dev = info;

	if (algt->alg.crypto.cra_flags & CRYPTO_ALG_NEED_FALLBACK) {
		CRYPTO_MSG("alloc fallback tfm, name = %s", alg_name);
		ctx->fallback_tfm = crypto_alloc_skcipher(alg_name, 0,
							  CRYPTO_ALG_ASYNC |
							  CRYPTO_ALG_NEED_FALLBACK);
		if (IS_ERR(ctx->fallback_tfm)) {
			dev_err(info->dev, "Could not load fallback driver %s : %ld.\n",
				alg_name, PTR_ERR(ctx->fallback_tfm));
			return PTR_ERR(ctx->fallback_tfm);
		}
	}

	return 0;
}

static void rk_ablk_cra_exit(struct crypto_tfm *tfm)
{
	struct rk_cipher_ctx *ctx = crypto_tfm_ctx(tfm);

	CRYPTO_TRACE();

	/* clear BC_CTL */
	CRYPTO_WRITE(ctx->dev, CRYPTO_BC_CTL, 0 | CRYPTO_WRITE_MASK_ALL);

	if (ctx->fallback_tfm) {
		CRYPTO_MSG("free fallback tfm");
		crypto_free_skcipher(ctx->fallback_tfm);
	}

	ctx->dev->release_crypto(ctx->dev, crypto_tfm_alg_name(tfm));
}

int rk_hw_crypto_v2_init(struct device *dev, void *hw_info)
{
	int err = 0;
	struct rk_hw_crypto_v2_info *info =
		(struct rk_hw_crypto_v2_info *)hw_info;

	info->desc = dma_alloc_coherent(dev,
					sizeof(struct crypto_lli_desc),
					&info->desc_dma,
					GFP_KERNEL);
	if (!info->desc) {
		err = -ENOMEM;
		goto end;
	}

end:
	return err;
}

void rk_hw_crypto_v2_deinit(struct device *dev, void *hw_info)
{
	struct rk_hw_crypto_v2_info *info =
		(struct rk_hw_crypto_v2_info *)hw_info;

	if (info && info->desc)
		dma_free_coherent(dev, sizeof(struct crypto_lli_desc),
				  info->desc, info->desc_dma);
}

struct rk_crypto_tmp rk_v2_ecb_sm4_alg =
	RK_CIPHER_ALGO_INIT(SM4, ECB, ecb(sm4), ecb-sm4-rk);

struct rk_crypto_tmp rk_v2_cbc_sm4_alg =
	RK_CIPHER_ALGO_INIT(SM4, CBC, cbc(sm4), cbc-sm4-rk);

struct rk_crypto_tmp rk_v2_xts_sm4_alg =
	RK_CIPHER_ALGO_XTS_INIT(SM4, xts(sm4), xts-sm4-rk);

struct rk_crypto_tmp rk_v2_cfb_sm4_alg =
	RK_CIPHER_ALGO_INIT(SM4, CFB, cfb(sm4), cfb-sm4-rk);

struct rk_crypto_tmp rk_v2_ofb_sm4_alg =
	RK_CIPHER_ALGO_INIT(SM4, OFB, ofb(sm4), ofb-sm4-rk);

struct rk_crypto_tmp rk_v2_ctr_sm4_alg =
	RK_CIPHER_ALGO_INIT(SM4, CTR, ctr(sm4), ctr-sm4-rk);

struct rk_crypto_tmp rk_v2_ecb_aes_alg =
	RK_CIPHER_ALGO_INIT(AES, ECB, ecb(aes), ecb-aes-rk);

struct rk_crypto_tmp rk_v2_cbc_aes_alg =
	RK_CIPHER_ALGO_INIT(AES, CBC, cbc(aes), cbc-aes-rk);

struct rk_crypto_tmp rk_v2_xts_aes_alg =
	RK_CIPHER_ALGO_XTS_INIT(AES, xts(aes), xts-aes-rk);

struct rk_crypto_tmp rk_v2_cfb_aes_alg =
	RK_CIPHER_ALGO_INIT(AES, CFB, cfb(aes), cfb-aes-rk);

struct rk_crypto_tmp rk_v2_ofb_aes_alg =
	RK_CIPHER_ALGO_INIT(AES, OFB, ofb(aes), ofb-aes-rk);

struct rk_crypto_tmp rk_v2_ctr_aes_alg =
	RK_CIPHER_ALGO_INIT(AES, CTR, ctr(aes), ctr-aes-rk);

struct rk_crypto_tmp rk_v2_ecb_des_alg =
	RK_CIPHER_ALGO_INIT(DES, ECB, ecb(des), ecb-des-rk);

struct rk_crypto_tmp rk_v2_cbc_des_alg =
	RK_CIPHER_ALGO_INIT(DES, CBC, cbc(des), cbc-des-rk);

struct rk_crypto_tmp rk_v2_cfb_des_alg =
	RK_CIPHER_ALGO_INIT(DES, CFB, cfb(des), cfb-des-rk);

struct rk_crypto_tmp rk_v2_ofb_des_alg =
	RK_CIPHER_ALGO_INIT(DES, OFB, ofb(des), ofb-des-rk);

struct rk_crypto_tmp rk_v2_ecb_des3_ede_alg =
	RK_CIPHER_ALGO_INIT(DES3_EDE, ECB, ecb(des3_ede), ecb-des3_ede-rk);

struct rk_crypto_tmp rk_v2_cbc_des3_ede_alg =
	RK_CIPHER_ALGO_INIT(DES3_EDE, CBC, cbc(des3_ede), cbc-des3_ede-rk);

struct rk_crypto_tmp rk_v2_cfb_des3_ede_alg =
	RK_CIPHER_ALGO_INIT(DES3_EDE, CFB, cfb(des3_ede), cfb-des3_ede-rk);

struct rk_crypto_tmp rk_v2_ofb_des3_ede_alg =
	RK_CIPHER_ALGO_INIT(DES3_EDE, OFB, ofb(des3_ede), ofb-des3_ede-rk);

