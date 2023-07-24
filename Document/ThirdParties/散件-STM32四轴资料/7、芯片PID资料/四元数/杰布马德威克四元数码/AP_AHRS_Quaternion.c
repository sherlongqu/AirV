/*
	
ap_ahrs_quaternion���룬���ڽܲ����������Ԫ����

����http://www.x-io.co.uk/res/doc/madgwick_internal_report.pdf

����APM������Tridgell����������뷨��

�Ӽ�˹͢��ɽë鷵����ۺ�ԭ�͡�

������������������������·����/��

�޸�����GNUͨ�ù���������

���֤�������������ᷢ���ģ���

2.1������֤���򣨸������ѡ���κ�ʱ��

�档
*/	
#include <FastSerial.h>
#include <AP_AHRS.h>

// to keep the code as close to the original as possible, we use these
// macros for quaternion access
#define SEq_1 q.q1
#define SEq_2 q.q2
#define SEq_3 q.q3
#define SEq_4 q.q4

// ����������һ�����ĵ���û�д�ǿ��

void AP_AHRS_Quaternion::update_IMU(float deltat, Vector3f &gyro, Vector3f &accel)
{
    // Local system variables
    float norm;                                                            // vector norm
    float SEqDot_omega_1, SEqDot_omega_2, SEqDot_omega_3, SEqDot_omega_4; // quaternion derrivative from gyroscopes elements
    float f_1, f_2, f_3;                                                   // objective function elements
    float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33;              // objective function Jacobian elements
    float SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4;              // estimated direction of the gyroscope error

    // Axulirary variables to avoid reapeated calcualtions
    float halfSEq_1 = 0.5f * SEq_1;
    float halfSEq_2 = 0.5f * SEq_2;
    float halfSEq_3 = 0.5f * SEq_3;
    float halfSEq_4 = 0.5f * SEq_4;
    float twoSEq_1 = 2.0f * SEq_1;
    float twoSEq_2 = 2.0f * SEq_2;
    float twoSEq_3 = 2.0f * SEq_3;

    // 	���Ƶ��������򣨻��ȣ�
    
    Vector3f w_err;

    // 	���������ٶ�ʸ��
    
    accel.normalize();
    if (accel.is_inf()) {
	    // discard this data point
	    renorm_range_count++;
	    return;
    }

    // 	����Ŀ�꺯�����ſɱȾ���
    
    f_1 = twoSEq_2 * SEq_4 - twoSEq_1 * SEq_3 - accel.x;
    f_2 = twoSEq_1 * SEq_2 + twoSEq_3 * SEq_4 - accel.y;
    f_3 = 1.0f - twoSEq_2 * SEq_2 - twoSEq_3 * SEq_3 - accel.z;
    J_11or24 = twoSEq_3;                                     // J_11 negated in matrix multiplication
    J_12or23 = 2.0f * SEq_4;
    J_13or22 = twoSEq_1;                                     // J_12 negated in matrix multiplication
    J_14or21 = twoSEq_2;
    J_32 = 2.0f * J_14or21;                                  // negated in matrix multiplication
    J_33 = 2.0f * J_11or24;                                  // negated in matrix multiplication

    // 	�ݶȵļ��㣨����˷���
    
    SEqHatDot_1 = J_14or21 * f_2 - J_11or24 * f_1;
    SEqHatDot_2 = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3;
    SEqHatDot_3 = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1;
    SEqHatDot_4 = J_14or21 * f_1 + J_11or24 * f_2;

    // 	���������ݶ�
    
    norm = 1.0/safe_sqrt(SEqHatDot_1 * SEqHatDot_1 + SEqHatDot_2 * SEqHatDot_2 + SEqHatDot_3 * SEqHatDot_3 + SEqHatDot_4 * SEqHatDot_4);
    if (isinf(norm)) {
	    // we can't do an update - discard this data point and
	    // hope the next one is better
	    renorm_range_count++;
	    return;
    }
    SEqHatDot_1 *= norm;
    SEqHatDot_2 *= norm;
    SEqHatDot_3 *= norm;
    SEqHatDot_4 *= norm;

    // 	������Ԫ��derrivative�Ĳ���������
    
    SEqDot_omega_1   = -halfSEq_2 * gyro.x - halfSEq_3 * gyro.y - halfSEq_4 * gyro.z;
    SEqDot_omega_2   = halfSEq_1  * gyro.x + halfSEq_3 * gyro.z - halfSEq_4 * gyro.y;
    SEqDot_omega_3   = halfSEq_1  * gyro.y - halfSEq_2 * gyro.z + halfSEq_4 * gyro.x;
    SEqDot_omega_4   = halfSEq_1  * gyro.z + halfSEq_2 * gyro.y - halfSEq_3 * gyro.x;

    //	����Ȼ�󽫹��Ƶ���Ԫ��derrivative
    
    SEq_1 += (SEqDot_omega_1 - (beta * SEqHatDot_1)) * deltat;
    SEq_2 += (SEqDot_omega_2 - (beta * SEqHatDot_2)) * deltat;
    SEq_3 += (SEqDot_omega_3 - (beta * SEqHatDot_3)) * deltat;
    SEq_4 += (SEqDot_omega_4 - (beta * SEqHatDot_4)) * deltat;

    // Normalise quaternion
    norm = 1.0/safe_sqrt(SEq_1 * SEq_1 + SEq_2 * SEq_2 + SEq_3 * SEq_3 + SEq_4 * SEq_4);
    if (isinf(norm)) {
	    // our quaternion is bad! Reset based on roll/pitch/yaw
	    // and hope for the best ...
	    renorm_blowup_count++;
	    if (_compass) {
		    _compass->null_offsets_disable();
	    }
	    q.from_euler(roll, pitch, yaw);
	    if (_compass) {
		    _compass->null_offsets_enable();
	    }
	    return;
    }
    SEq_1 *= norm;
    SEq_2 *= norm;
    SEq_3 *= norm;
    SEq_4 *= norm;
}


// ����������һ�����ĵ���������ǿ��

void AP_AHRS_Quaternion::update_MARG(float deltat, Vector3f &gyro, Vector3f &accel, Vector3f &mag)
{
    // local system variables
    float norm;                                                             //                vector norm
    float SEqDot_omega_1, SEqDot_omega_2, SEqDot_omega_3, SEqDot_omega_4;   //             �������ǵ�Ԫ������
    
    float f_1, f_2, f_3, f_4, f_5, f_6;                                     //               Ŀ�꺯����Ԫ��
    
    float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33,               //                Ŀ�꺯�����ſɱȾ���Ԫ��
    
    J_41, J_42, J_43, J_44, J_51, J_52, J_53, J_54, J_61, J_62, J_63, J_64; //
    float SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4;               //              	���Ƶ���������
    

    // 	�����ͨ���ڵ���Ŀ��
    
    Vector3f flux;

    // 	���Ƶ��������򣨻��ȣ�
    
    Vector3f w_err;

    // 	���������ٶ�ʸ��
    
    accel.normalize();
    if (accel.is_inf()) {
	    // 		����������ݵ�
	    
	    renorm_range_count++;
	    return;
    }

    // 	�������Ĵ�ǿ�Ʋ���
    
    mag.normalize();
    if (mag.is_inf()) {
	    //����������ݵ�
	    
	    renorm_range_count++;
	    return;
    }

    //	Ϊ�˱����ظ�����ĸ�������
    
    float halfSEq_1 = 0.5f * SEq_1;
    float halfSEq_2 = 0.5f * SEq_2;
    float halfSEq_3 = 0.5f * SEq_3;
    float halfSEq_4 = 0.5f * SEq_4;
    float twoSEq_1 = 2.0f * SEq_1;
    float twoSEq_2 = 2.0f * SEq_2;
    float twoSEq_3 = 2.0f * SEq_3;
    float twoSEq_4 = 2.0f * SEq_4;
    float twob_x = 2.0f * b_x;
    float twob_z = 2.0f * b_z;
    float twob_xSEq_1 = 2.0f * b_x * SEq_1;
    float twob_xSEq_2 = 2.0f * b_x * SEq_2;
    float twob_xSEq_3 = 2.0f * b_x * SEq_3;
    float twob_xSEq_4 = 2.0f * b_x * SEq_4;
    float twob_zSEq_1 = 2.0f * b_z * SEq_1;
    float twob_zSEq_2 = 2.0f * b_z * SEq_2;
    float twob_zSEq_3 = 2.0f * b_z * SEq_3;
    float twob_zSEq_4 = 2.0f * b_z * SEq_4;
    float SEq_1SEq_2;
    float SEq_1SEq_3 = SEq_1 * SEq_3;
    float SEq_1SEq_4;
    float SEq_2SEq_3;
    float SEq_2SEq_4 = SEq_2 * SEq_4;
    float SEq_3SEq_4;
    Vector3f twom = mag * 2.0;

    // 	����Ŀ�꺯�����ſɱȾ���
    
    f_1 = twoSEq_2 * SEq_4 - twoSEq_1 * SEq_3 - accel.x;
    f_2 = twoSEq_1 * SEq_2 + twoSEq_3 * SEq_4 - accel.y;
    f_3 = 1.0f - twoSEq_2 * SEq_2 - twoSEq_3 * SEq_3 - accel.z;
    f_4 = twob_x * (0.5f - SEq_3 * SEq_3 - SEq_4 * SEq_4) + twob_z * (SEq_2SEq_4 - SEq_1SEq_3) - mag.x;
    f_5 = twob_x * (SEq_2 * SEq_3 - SEq_1 * SEq_4) + twob_z * (SEq_1 * SEq_2 + SEq_3 * SEq_4) - mag.y;
    f_6 = twob_x * (SEq_1SEq_3 + SEq_2SEq_4) + twob_z * (0.5f - SEq_2 * SEq_2 - SEq_3 * SEq_3) - mag.z;
    J_11or24 = twoSEq_3;                                  // 11�񶨾���˷�
    J_12or23 = 2.0f * SEq_4;
    J_13or22 = twoSEq_1;                                  // J_12 negated in matrix multiplication
    J_14or21 = twoSEq_2;
    J_32 = 2.0f * J_14or21;                               // 	�񶨵ľ���˷�
    
    J_33 = 2.0f * J_11or24;                               // negated in matrix multiplication
    J_41 = twob_zSEq_3;                                   // negated in matrix multiplication
    J_42 = twob_zSEq_4;
    J_43 = 2.0f * twob_xSEq_3 + twob_zSEq_1;              // negated in matrix multiplication
    J_44 = 2.0f * twob_xSEq_4 - twob_zSEq_2;              // negated in matrix multiplication
    J_51 = twob_xSEq_4 - twob_zSEq_2;                     // negated in matrix multiplication
    J_52 = twob_xSEq_3 + twob_zSEq_1;
    J_53 = twob_xSEq_2 + twob_zSEq_4;
    J_54 = twob_xSEq_1 - twob_zSEq_3;                     // negated in matrix multiplication
    J_61 = twob_xSEq_3;
    J_62 = twob_xSEq_4 - 2.0f * twob_zSEq_2;
    J_63 = twob_xSEq_1 - 2.0f * twob_zSEq_3;
    J_64 = twob_xSEq_2;

    // 	�ݶȵļ��㣨����˷���
    
    SEqHatDot_1 = J_14or21 * f_2 - J_11or24 * f_1              - J_41 * f_4 - J_51 * f_5 + J_61 * f_6;
    SEqHatDot_2 = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3 + J_42 * f_4 + J_52 * f_5 + J_62 * f_6;
    SEqHatDot_3 = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1 - J_43 * f_4 + J_53 * f_5 + J_63 * f_6;
    SEqHatDot_4 = J_14or21 * f_1 + J_11or24 * f_2              - J_44 * f_4 - J_54 * f_5 + J_64 * f_6;

    // 	�������ݶȶ������������
    
    norm = 1.0 / safe_sqrt(SEqHatDot_1 * SEqHatDot_1 + SEqHatDot_2 * SEqHatDot_2 + SEqHatDot_3 * SEqHatDot_3 + SEqHatDot_4 * SEqHatDot_4);
    if (isinf(norm)) {
	    // discard this data point
	    renorm_range_count++;
	    return;
    }
    SEqHatDot_1 *= norm;
    SEqHatDot_2 *= norm;
    SEqHatDot_3 *= norm;
    SEqHatDot_4 *= norm;

    // 	�������������ǹ��Ƶķ���
    
    w_err.x = twoSEq_1 * SEqHatDot_2 - twoSEq_2 * SEqHatDot_1 - twoSEq_3 * SEqHatDot_4 + twoSEq_4 * SEqHatDot_3;
    w_err.y = twoSEq_1 * SEqHatDot_3 + twoSEq_2 * SEqHatDot_4 - twoSEq_3 * SEqHatDot_1 - twoSEq_4 * SEqHatDot_2;
    w_err.z = twoSEq_1 * SEqHatDot_4 - twoSEq_2 * SEqHatDot_3 + twoSEq_3 * SEqHatDot_2 - twoSEq_4 * SEqHatDot_1;

    //	���������
    
    _error_rp_sum += 0.5*(fabs(w_err.x) + fabs(w_err.y));
    _error_yaw_sum += fabs(w_err.z);
    _error_rp_count++;
    _error_yaw_count++;

    // 	����������ƫ��������
    
    Vector3f drift_delta = w_err * (deltat * zeta);

  
  //������Ư���ʽ����������ֹ�� 
  //ͻȻƯ�Ʊ仯����������һ���ж�
  
    float max_change = _gyro_drift_limit * deltat;
    drift_delta.x = constrain(drift_delta.x, -max_change, max_change);
    drift_delta.y = constrain(drift_delta.y, -max_change, max_change);
    drift_delta.z = constrain(drift_delta.z, -max_change, max_change);
    gyro_bias += drift_delta;

    // 	��ȷ������Ư�Ƶ��Ķ�
    
    gyro -= gyro_bias;

    // 	������Ԫ���ʵĲ���������
    
    SEqDot_omega_1 = -halfSEq_2 * gyro.x - halfSEq_3 * gyro.y - halfSEq_4 * gyro.z;
    SEqDot_omega_2 = halfSEq_1  * gyro.x + halfSEq_3 * gyro.z - halfSEq_4 * gyro.y;
    SEqDot_omega_3 = halfSEq_1  * gyro.y - halfSEq_2 * gyro.z + halfSEq_4 * gyro.x;
    SEqDot_omega_4 = halfSEq_1  * gyro.z + halfSEq_2 * gyro.y - halfSEq_3 * gyro.x;

    // 	Ȼ�󽫹��Ƶ���Ԫ���ʵļ���
    
    SEq_1 += (SEqDot_omega_1 - (beta * SEqHatDot_1)) * deltat;
    SEq_2 += (SEqDot_omega_2 - (beta * SEqHatDot_2)) * deltat;
    SEq_3 += (SEqDot_omega_3 - (beta * SEqHatDot_3)) * deltat;
    SEq_4 += (SEqDot_omega_4 - (beta * SEqHatDot_4)) * deltat;

    // 	����������Ԫ��
    
    norm = 1.0/safe_sqrt(SEq_1 * SEq_1 + SEq_2 * SEq_2 + SEq_3 * SEq_3 + SEq_4 * SEq_4);
    if (isinf(norm)) {
	    
		//���ǵ���Ԫ�����λ���ڹ�/����/ƫ��
		//����õ�ϣ����
	    
	    renorm_blowup_count++;
	    _compass->null_offsets_disable();
	    q.from_euler(roll, pitch, yaw);
	    _compass->null_offsets_disable();
	    return;
    }
    SEq_1 *= norm;
    SEq_2 *= norm;
    SEq_3 *= norm;
    SEq_4 *= norm;

    
	//����ͨ���ڵ���Ŀ��
	//���¼���axulirary����
    
    SEq_1SEq_2 = SEq_1 * SEq_2;
    SEq_1SEq_3 = SEq_1 * SEq_3;
    SEq_1SEq_4 = SEq_1 * SEq_4;
    SEq_3SEq_4 = SEq_3 * SEq_4;
    SEq_2SEq_3 = SEq_2 * SEq_3;
    SEq_2SEq_4 = SEq_2 * SEq_4;
    flux.x = twom.x * (0.5f - SEq_3 * SEq_3 - SEq_4 * SEq_4) + twom.y * (SEq_2SEq_3 - SEq_1SEq_4) + twom.z * (SEq_2SEq_4 + SEq_1SEq_3);
    flux.y = twom.x * (SEq_2SEq_3 + SEq_1SEq_4) + twom.y * (0.5f - SEq_2 * SEq_2 - SEq_4 * SEq_4) + twom.z * (SEq_3SEq_4 - SEq_1SEq_2);
    flux.z = twom.x * (SEq_2SEq_4 - SEq_1SEq_3) + twom.y * (SEq_3SEq_4 + SEq_1SEq_2) + twom.z * (0.5f - SEq_2 * SEq_2 - SEq_3 * SEq_3);

    // 	�������Ĵ�ͨʸ����X��Z�����
    
    b_x = sqrt((flux.x * flux.x) + (flux.y * flux.y));
    b_z = flux.z;
}


// ����������һ�����ĵ���

void AP_AHRS_Quaternion::update(void)
{
	Vector3f gyro, accel;
	float deltat;

	_imu->update();

	deltat = _imu->get_delta_time();
	if (deltat > 1.0) {
		
		//�������ֹͣ����Ϊ1s������Ӧ�÷�����
		//�������ݡ�������������еķ���
		
		//����������£�����������ݵ�
		
		//���������̬����һ���޴������
		
		return;
	}

	if (!_have_initial_yaw && _compass &&
	    _compass->use_for_yaw()) {
		// setup the quaternion with initial compass yaw
		_compass->null_offsets_disable();
		q.from_euler(0, 0, _compass->heading);
		_have_initial_yaw = true;
		_compass_last_update = _compass->last_update;
		gyro_bias.zero();
		_compass->null_offsets_enable();
	}

	// get current IMU state
	gyro = _imu->get_gyro();

	//	��Ԫ��ϵͳʹ�÷����෴�ļ���
	
	accel = - _imu->get_accel();

	if (_centripetal && _gps && _gps->status() == GPS::GPS_OK) {
		// compensate for linear acceleration. This makes a
		// surprisingly large difference in the pitch estimate when
		// turning, plus on takeoff and landing
		float acceleration = _gps->acceleration();
		accel.x += acceleration;

		// �������ļ��ٶ�
		
		float veloc;
		veloc = _gps->ground_speed * 0.01;
		
		//С�ı��ļ���ļ��󡣵�
		
		//��Ԫϵͳʹ�ò�ͬ�ļ����
		
		//��ϢAPM
		
		accel.y += (gyro.z - gyro_bias.z) * veloc;
		accel.z -= (gyro.y - gyro_bias.y) * veloc;
	}

	if (_compass != NULL && _compass->use_for_yaw()) {
		Vector3f mag = Vector3f(_compass->mag_x, _compass->mag_y, _compass->mag_z);
		update_MARG(deltat, gyro, accel, mag);
	} else {
		// ʹ�����ݺ�accels��Ԫ���Ľ������
		
		gyro -= gyro_bias;
		update_IMU(deltat, gyro, accel);
	}

#ifdef DESKTOP_BUILD
	if (q.is_nan()) {
		SITL_debug("QUAT NAN: deltat=%f roll=%f pitch=%f yaw=%f q=[%f %f %f %f] a=[%f %f %f] g=(%f %f %f)\n",
			   deltat, roll, pitch, yaw,
			   q.q1, q.q2, q.q3, q.q4,
			   accel.x, accel.y, accel.z,
			   gyro.x, gyro.y, gyro.z);
	}
#endif

	// 	����У�����ݵı���
	
	_gyro_corrected = gyro;

	// calculate our euler angles for high level control and navigation
	q.to_euler(&roll, &pitch, &yaw);

	// the code above assumes zero magnetic declination, so offset
	// the yaw here
	if (_compass != NULL) {
		yaw += _compass->get_declination();
	}

	// and integer Eulers
	roll_sensor  = 100 * ToDeg(roll);
	pitch_sensor = 100 * ToDeg(pitch);
	yaw_sensor   = 100 * ToDeg(yaw);
	if (yaw_sensor < 0) {
		yaw_sensor += 36000;
	}

}

// average error in roll/pitch since last call
float AP_AHRS_Quaternion::get_error_rp(void)
{
	float ret;
	if (_error_rp_count == 0) {
		return 0;
	}
	ret = _error_rp_sum / _error_rp_count;
	_error_rp_sum = 0;
	_error_rp_count = 0;
	return ret;
}

// average error in yaw since last call
float AP_AHRS_Quaternion::get_error_yaw(void)
{
	float ret;
	if (_error_yaw_count == 0) {
		return 0;
	}
	ret = _error_yaw_sum / _error_yaw_count;
	_error_yaw_sum = 0;
	_error_yaw_count = 0;
	return ret;
}

// reset attitude system
void AP_AHRS_Quaternion::reset(bool recover_eulers)
{
	if (recover_eulers) {
		q.from_euler(roll, pitch, yaw);
	} else {
		q(1, 0, 0, 0);
	}
	gyro_bias.zero();

	// reference direction of flux in earth frame
	b_x = 0;
	b_z = -1;
}
