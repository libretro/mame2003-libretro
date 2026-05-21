void *wav_open(const char *filename, int sample_rate, int channels);
void wav_close(void *wavptr);

void wav_add_data_16(void *wavptr, int16_t *data, int samples);
void wav_add_data_32(void *wavptr, int32_t *data, int samples, int shift);
void wav_add_data_16lr(void *wavptr, int16_t *left, int16_t *right, int samples);
void wav_add_data_32lr(void *wavptr, int32_t *left, int32_t *right, int samples, int shift);
