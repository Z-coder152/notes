#ifndef MPU6050_HHHH
#define MPU6050_HHHH

#define MPU6050_MAGIC 'K'


/*
 *ʹ���������һ�����ã���ʡ�ڴ�ռ�
 *����������г�Ա����ͬһ���ڴ�ռ䣬
 *������Ĵ�С�������ĳ�Ա�Ĵ�С
 */
union mpu6050_data 
{
	struct {
		unsigned short x;
		unsigned short y;
		unsigned short z;
	}accel; //������ٶ�
	struct {
		unsigned short x;
		unsigned short y;
		unsigned short z;
	}gyro; //����������(������ٶ�)
	unsigned short temp; //�¶�ֵ
};

#define GET_ACCEL _IOR(MPU6050_MAGIC, 0, union mpu6050_data)
#define GET_GYRO  _IOR(MPU6050_MAGIC, 1, union mpu6050_data) 
#define GET_TEMP  _IOR(MPU6050_MAGIC, 2, union mpu6050_data)

#endif