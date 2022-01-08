// *****************************************************************************
//
// © Copyright 2020, Septentrio NV/SA.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//    1. Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//    2. Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//    3. Neither the name of the copyright holder nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// *****************************************************************************

// *****************************************************************************
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:

// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// *****************************************************************************

//! 0x24 is ASCII for $ - 1st byte in each message
#ifndef SBF_SYNC_BYTE_1
#define SBF_SYNC_BYTE_1 0x24
#endif
//! 0x40 is ASCII for @ - 2nd byte to indicate SBF block
#ifndef SBF_SYNC_BYTE_2
#define SBF_SYNC_BYTE_2 0x40
#endif
//! 0x24 is ASCII for $ - 1st byte in each message
#ifndef NMEA_SYNC_BYTE_1
#define NMEA_SYNC_BYTE_1 0x24
#endif
//! 0x47 is ASCII for G - 2nd byte to indicate NMEA-type ASCII message
#ifndef NMEA_SYNC_BYTE_2_1
#define NMEA_SYNC_BYTE_2_1 0x47
#endif
//! 0x50 is ASCII for P - 2nd byte to indicate proprietary ASCII message
#ifndef NMEA_SYNC_BYTE_2_2
#define NMEA_SYNC_BYTE_2_2 0x50
#endif
//! 0x24 is ASCII for $ - 1st byte in each response from the Rx
#ifndef RESPONSE_SYNC_BYTE_1
#define RESPONSE_SYNC_BYTE_1 0x24
#endif
//! 0x52 is ASCII for R (for "Response") - 2nd byte in each response from the Rx
#ifndef RESPONSE_SYNC_BYTE_2
#define RESPONSE_SYNC_BYTE_2 0x52
#endif
//! 0x0D is ASCII for "Carriage Return", i.e. "Enter"
#ifndef CARRIAGE_RETURN
#define CARRIAGE_RETURN 0x0D
#endif
//! 0x0A is ASCII for "Line Feed", i.e. "New Line"
#ifndef LINE_FEED
#define LINE_FEED 0x0A
#endif
//! 0x3F is ASCII for ? - 3rd byte in the response message from the Rx in case the
//! command was invalid
#ifndef RESPONSE_SYNC_BYTE_3
#define RESPONSE_SYNC_BYTE_3 0x3F
#endif
//! 0x49 is ASCII for I - 1st character of connection descriptor sent by the Rx after
//! initiating TCP connection
#ifndef CONNECTION_DESCRIPTOR_BYTE_1
#define CONNECTION_DESCRIPTOR_BYTE_1 0x49
#endif
//! 0x50 is ASCII for P - 2nd character of connection descriptor sent by the Rx after
//! initiating TCP connection
#ifndef CONNECTION_DESCRIPTOR_BYTE_2
#define CONNECTION_DESCRIPTOR_BYTE_2 0x50
#endif

// C++ libraries
#include <cassert> // for assert
#include <cstddef>
#include <map>
#include <sstream>
// Boost includes
#include <boost/call_traits.hpp>
#include <boost/format.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/tokenizer.hpp>
// ROSaic includes
#include <septentrio_gnss_driver/abstraction/typedefs.hpp>
#include <septentrio_gnss_driver/crc/crc.h>
#include <septentrio_gnss_driver/parsers/nmea_parsers/gpgga.hpp>
#include <septentrio_gnss_driver/parsers/nmea_parsers/gpgsa.hpp>
#include <septentrio_gnss_driver/parsers/nmea_parsers/gpgsv.hpp>
#include <septentrio_gnss_driver/parsers/nmea_parsers/gprmc.hpp>
#include <septentrio_gnss_driver/parsers/string_utilities.h>

#ifndef RX_MESSAGE_HPP
#define RX_MESSAGE_HPP

/**
 * @file rx_message.hpp
 * @date 20/08/20
 * @brief Defines a class that reads messages handed over from the circular buffer
 */

extern bool g_read_cd;
extern uint32_t g_cd_count;

//! Settings struct
struct Settings
{
    //! Device port
    std::string device;
    //! Delay in seconds between reconnection attempts to the connection type
    //! specified in the parameter connection_type
    float reconnect_delay_s;
    //! Baudrate
    uint32_t baudrate;
    //! HW flow control
    std::string hw_flow_control;
    //! In case of serial communication to Rx, rx_serial_port specifies Rx's
    //! serial port connected to, e.g. USB1 or COM1
    std::string rx_serial_port;
    //! Datum to be used
    std::string datum;
    //! Polling period for PVT-related SBF blocks
    uint32_t polling_period_pvt;
    //! Polling period for all other SBF blocks and NMEA messages
    uint32_t polling_period_rest;
    //! Marker-to-ARP offset in the eastward direction
    float delta_e;
    //! Marker-to-ARP offset in the northward direction
    float delta_n;
    //! Marker-to-ARP offset in the upward direction
    float delta_u;
    //! Main antenna type, from the list returned by the command "lstAntennaInfo,
    //! Overview"
    std::string ant_type;
    //! Aux1 antenna type, from the list returned by the command "lstAntennaInfo,
    //! Overview"
    std::string ant_aux1_type;
    //! Serial number of your particular Main antenna
    std::string ant_serial_nr;
    //! Serial number of your particular Aux1 antenna
    std::string ant_aux1_serial_nr;
    //! ROS axis orientation, body: front-left-up, geographic: ENU
    bool use_ros_axis_orientation;
    //! IMU orientation x-angle
    double theta_x;
    //! IMU orientation y-angle
    double theta_y;
    //! IMU orientation z-angle
    double theta_z;
    //! INS antenna lever arm x-offset
    double ant_lever_x;
    //! INS antenna lever arm y-offset
    double ant_lever_y;
    //! INS antenna lever arm z-offset
    double ant_lever_z;
    //! INS POI offset in x-dimension
    double poi_x;
    //! INS POI offset in y-dimension
    double poi_y;
    //! INS POI offset in z-dimension
    double poi_z;
    //! INS velocity sensor lever arm x-offset
    double vsm_x;
    //! INS velocity sensor lever arm y-offset
    double vsm_y;
    //! INS velocity sensor lever arm z-offset
    double vsm_z;
    //! Attitude offset determination in longitudinal direction
    double heading_offset;
    //! Attitude offset determination in latitudinal direction
    double pitch_offset;
    //! INS multiantenna
    bool multi_antenna;
    //! INS solution reference point
    bool ins_use_poi;
    //! For heading computation when unit is powered-cycled
    std::string ins_initial_heading;
    //! Attitude deviation mask
    float att_std_dev;
    //! Position deviation mask
    float pos_std_dev;
    //! Type of NTRIP connection
    std::string ntrip_mode;
    //! Hostname or IP address of the NTRIP caster to connect to
    std::string caster;
    //! IP port number of NTRIP caster to connect to
    uint32_t caster_port;
    //! Username for NTRIP service
    std::string ntrip_username;
    //! Password for NTRIP service
    std::string ntrip_password;
    //! Mountpoint for NTRIP service
    std::string mountpoint;
    //! NTRIP version for NTRIP service
    std::string ntrip_version;
    //! Whether Rx has internet or not
    bool rx_has_internet;
    //! RTCM version for NTRIP service (if Rx does not have internet)
    std::string rtcm_version;
    //! Rx TCP port number, e.g. 28785, on which Rx receives the corrections
    //! (can't be the same as main connection unless localhost concept is used)
    uint32_t rx_input_corrections_tcp;
    //! Rx serial port, e.g. USB2, on which Rx receives the corrections (can't be
    //! the same as main connection unless localhost concept is used)
    std::string rx_input_corrections_serial;
    //! Whether (and at which rate) or not to send GGA to the NTRIP caster
    std::string send_gga;
    //! Whether or not to publish the GGA message
    bool publish_gpgga;
    //! Whether or not to publish the RMC message
    bool publish_gprmc;
    //! Whether or not to publish the GSA message
    bool publish_gpgsa;
    //! Whether or not to publish the GSV message
    bool publish_gpgsv;
    //! Whether or not to publish the PVTCartesianMsg
    //! message
    bool publish_pvtcartesian;
    //! Whether or not to publish the PVTGeodeticMsg message
    bool publish_pvtgeodetic;
    //! Whether or not to publish the PosCovCartesianMsg
    //! message
    bool publish_poscovcartesian;
    //! Whether or not to publish the PosCovGeodeticMsg
    //! message
    bool publish_poscovgeodetic;
    //! Whether or not to publish the VelCovGeodeticMsg
    //! message
    bool publish_velcovgeodetic;
    //! Whether or not to publish the AttEulerMsg message
    bool publish_atteuler;
    //! Whether or not to publish the AttCovEulerMsg message
    bool publish_attcoveuler;
    //! Whether or not to publish the INSNavCartMsg message
    bool publish_insnavcart;
    //! Whether or not to publish the INSNavGeodMsg message
    bool publish_insnavgeod;
    //! Whether or not to publish the IMUSetupMsg message
    bool publish_imusetup;
    //! Whether or not to publish the VelSensorSetupMsg message
    bool publish_velsensorsetup;
    //! Whether or not to publish the ExtEventINSNavGeodMsg message
    bool publish_exteventinsnavgeod;
    //! Whether or not to publish the ExtEventINSNavCartMsg message
    bool publish_exteventinsnavcart;
    //! Whether or not to publish the ExtSensorMeasMsg message
    bool publish_extsensormeas;
    //! Whether or not to publish the TimeReferenceMsg message with GPST
    bool publish_gpst;
    //! Whether or not to publish the NavSatFixMsg message
    bool publish_navsatfix;
    //! Whether or not to publish the GPSFixMsg message
    bool publish_gpsfix;
    //! Whether or not to publish the PoseWithCovarianceStampedMsg message
    bool publish_pose;
    //! Whether or not to publish the DiagnosticArrayMsg message
    bool publish_diagnostics;
    //! Whether or not to publish the ImuMsg message
    bool publish_imu;
    //! Whether or not to publish the LocalizationMsg message
    bool publish_localization;
    //! Whether or not to publish the tf of the localization
    bool publish_tf;
    //! Septentrio receiver type, either "gnss" or "ins"
    std::string septentrio_receiver_type;
    //! If true, the ROS message headers' unix time field is constructed from the TOW (in
    //! the SBF case) and UTC (in the NMEA case) data. If false, times are constructed
    //! within the driver via time(NULL) of the \<ctime\> library.
    bool use_gnss_time;
    //! The frame ID used in the header of every published ROS message
    std::string frame_id;
    //! The frame ID used in the header of published ROS Imu message
    std::string imu_frame_id;
    //! The frame ID used in the header of published ROS Localization message if poi is used
    std::string poi_frame_id;
    //! The frame ID of the velocity sensor
    std::string vsm_frame_id;
    //! The frame ID of the aux1 antenna
    std::string aux1_frame_id;
    //! The frame ID of the vehicle frame
    std::string vehicle_frame_id;
    //! Wether the UTM zone of the localization is locked
    bool lock_utm_zone;
    //! The number of leap seconds that have been inserted into the UTC time
    uint32_t leap_seconds;
    //! Whether or not we are reading from an SBF file
    bool read_from_sbf_log;
    //! Whether or not we are reading from a PCAP file
    bool read_from_pcap;
};

//! Enum for NavSatFix's status.status field, which is obtained from PVTGeodetic's
//! Mode field
enum TypeOfPVT_Enum
{
    evNoPVT,
    evStandAlone,
    evDGPS,
    evFixed,
    evRTKFixed,
    evRTKFloat,
    evSBAS,
    evMovingBaseRTKFixed,
    evMovingBaseRTKFloat,
    evPPP
};

//! Since switch only works with int (yet NMEA message IDs are strings), we need
//! enum. Note drawbacks: No variable can have a name which is already in some
//! enumeration, enums are not type safe etc..
enum RxID_Enum
{
    evNavSatFix,
    evINSNavSatFix,
    evGPSFix,
    evINSGPSFix,
    evPoseWithCovarianceStamped,
    evINSPoseWithCovarianceStamped,
    evGPGGA,
    evGPRMC,
    evGPGSA,
    evGPGSV,
    evGLGSV,
    evGAGSV,
    evPVTCartesian,
    evPVTGeodetic,
    evPosCovCartesian,
    evPosCovGeodetic,
    evAttEuler,
    evAttCovEuler,
    evINSNavCart,
    evINSNavGeod,
    evIMUSetup,
    evVelSensorSetup,
    evExtEventINSNavGeod,
    evExtEventINSNavCart,
    evExtSensorMeas,
    evGPST,
    evChannelStatus,
    evMeasEpoch,
    evDOP,
    evVelCovGeodetic,
    evDiagnosticArray,
    evImu,
    evLocalization,
    evReceiverStatus,
    evQualityInd,
    evReceiverSetup
};

namespace io_comm_rx {
    /**
     * @brief Calculates the timestamp, in the Unix Epoch time format
     * This is either done using the TOW as transmitted with the SBF block (if
     * "use_gnss" is true), or using the current time.
     * @param[in] tow (Time of Week) Number of milliseconds that elapsed since the
     * beginning of the current GPS week as transmitted by the SBF block
     * @param[in] use_gnss If true, the TOW as transmitted with the SBF block is
     * used, otherwise the current time
     * @return ros::Time object containing seconds and nanoseconds since last epoch
     */
    ros::Time timestampSBF(uint32_t tow, uint16_t wnc, bool use_gnss);
   
    /**
     * @class RxMessage
     * @brief Can search buffer for messages, read/parse them, and so on
     */
    class RxMessage
    {
    public:
        /**
         * @brief Constructor of the RxMessage class
         *
         * One can always provide a non-const value where a const one was expected.
         * The const-ness of the argument just means the function promises not to
         * change it.. Recall: static_cast by the way can remove or add const-ness,
         * no other C++ cast is capable of removing it (not even reinterpret_cast)
         * @param[in] data Pointer to the buffer that is about to be analyzed
         * @param[in] size Size of the buffer (as handed over by async_read_some)
         */
        RxMessage(ROSaicNodeBase* node, Settings* settings) :
            node_(node),
            settings_(settings),
            unix_time_(0)
        {
            found_ = false;
            crc_check_ = false;
            message_size_ = 0;
                    
            //! Pair of iterators to facilitate initialization of the map
            std::pair<uint16_t, TypeOfPVT_Enum> type_of_pvt_pairs[] = {
            std::make_pair(static_cast<uint16_t>(0), evNoPVT),
            std::make_pair(static_cast<uint16_t>(1), evStandAlone),
            std::make_pair(static_cast<uint16_t>(2), evDGPS),
            std::make_pair(static_cast<uint16_t>(3), evFixed),
            std::make_pair(static_cast<uint16_t>(4), evRTKFixed),
            std::make_pair(static_cast<uint16_t>(5), evRTKFloat),
            std::make_pair(static_cast<uint16_t>(6), evSBAS),
            std::make_pair(static_cast<uint16_t>(7), evMovingBaseRTKFixed),
            std::make_pair(static_cast<uint16_t>(8), evMovingBaseRTKFloat),
            std::make_pair(static_cast<uint16_t>(10), evPPP)};

            type_of_pvt_map = TypeOfPVTMap(type_of_pvt_pairs, type_of_pvt_pairs + evPPP + 1);
            
            //! Pair of iterators to facilitate initialization of the map
            std::pair<std::string, RxID_Enum> rx_id_pairs[] = {
            std::make_pair("NavSatFix", evNavSatFix),
            std::make_pair("INSNavSatFix", evINSNavSatFix),   
            std::make_pair("GPSFix", evGPSFix),
            std::make_pair("INSGPSFix", evINSGPSFix),
            std::make_pair("PoseWithCovarianceStamped", evPoseWithCovarianceStamped),
            std::make_pair("INSPoseWithCovarianceStamped", evINSPoseWithCovarianceStamped),
            std::make_pair("$GPGGA", evGPGGA),
            std::make_pair("$GPRMC", evGPRMC),
            std::make_pair("$GPGSA", evGPGSA),
            std::make_pair("$GPGSV", evGPGSV),
            std::make_pair("$GLGSV", evGLGSV),
            std::make_pair("$GAGSV", evGAGSV),
            std::make_pair("4006", evPVTCartesian),
            std::make_pair("4007", evPVTGeodetic),
            std::make_pair("5905", evPosCovCartesian),
            std::make_pair("5906", evPosCovGeodetic),
            std::make_pair("5938", evAttEuler),
            std::make_pair("5939", evAttCovEuler),
            std::make_pair("GPST", evGPST),
            std::make_pair("4013", evChannelStatus),
            std::make_pair("4027", evMeasEpoch),
            std::make_pair("4001", evDOP),
            std::make_pair("5908", evVelCovGeodetic),
            std::make_pair("DiagnosticArray", evDiagnosticArray),
            std::make_pair("Imu", evImu),
            std::make_pair("Localization", evLocalization),
            std::make_pair("4014", evReceiverStatus),
            std::make_pair("4082", evQualityInd),
            std::make_pair("5902", evReceiverSetup),
            std::make_pair("4225", evINSNavCart),
            std::make_pair("4226", evINSNavGeod),
            std::make_pair("4230", evExtEventINSNavGeod),
            std::make_pair("4229", evExtEventINSNavCart),
            std::make_pair("4224", evIMUSetup),
            std::make_pair("4244", evVelSensorSetup),
            std::make_pair("4050", evExtSensorMeas)
            };

            rx_id_map = RxIDMap(rx_id_pairs, rx_id_pairs + evReceiverSetup + 1);
        }

         /**
         * @brief Put new data
         * @param[in] recvTimestamp Timestamp of receiving buffer
         * @param[in] data Pointer to the buffer that is about to be analyzed
         * @param[in] size Size of the buffer (as handed over by async_read_some)
         */
        void newData(Timestamp recvTimestamp, const uint8_t* data, std::size_t& size)
        {
            recvTimestamp_ = recvTimestamp;
            data_ = data;
            count_ = size;
            found_ = false;
            crc_check_ = false;
            message_size_ = 0;
        }

        //! Determines whether data_ points to the SBF block with ID "ID", e.g. 5003
        bool isMessage(const uint16_t ID);
        //! Determines whether data_ points to the NMEA message with ID "ID", e.g.
        //! "$GPGGA"
        bool isMessage(std::string ID);
        //! Determines whether data_ currently points to an SBF block
        bool isSBF();
        //! Determines whether data_ currently points to an NMEA message
        bool isNMEA();
        //! Determines whether data_ currently points to an NMEA message
        bool isResponse();
        //! Determines whether data_ currently points to a connection descriptor
        //! (right after initiating a TCP connection)
        bool isConnectionDescriptor();
        //! Determines whether data_ currently points to an error message reply from
        //! the Rx
        bool isErrorMessage();
        //! Determines size of the message (also command reply) that data_ is
        //! currently pointing at
        std::size_t messageSize();
        //! Returns the message ID of the message where data_ is pointing at at the
        //! moment, SBF identifiers embellished with inverted commas, e.g. "5003"
        std::string messageID();

        /**
         * @brief Returns the count_ variable
         * @return The variable count_
         */
        std::size_t getCount() { return count_; };
        /**
         * @brief Searches the buffer for the beginning of the next message (NMEA or
         * SBF)
         * @return A pointer to the start of the next message.
         */
        const uint8_t* search();

        /**
         * @brief Gets the length of the SBF block
         *
         * It determines the length from the header of the SBF block. The block
         * length thus includes the header length.
         * @return The length of the SBF block
         */
        uint16_t getBlockLength();

        /**
         * @brief Gets the current position in the read buffer
         * @return The current position of the read buffer
         */
        const uint8_t* getPosBuffer();

        /**
         * @brief Gets the end position in the read buffer
         * @return A pointer pointing to just after the read buffer (matches
         * search()'s final pointer in case no valid message is found)
         */
        const uint8_t* getEndBuffer();

        /**
         * @brief Has an NMEA message, SBF block or command reply been found in the
         * buffer?
         * @returns True if a message with the correct header & length has been found
         */
        bool found();

        /**
         * @brief Goes to the start of the next message based on the calculated
         * length of current message
         */
        void next();

        /**
         * @brief Performs the CRC check (if SBF) and publishes ROS messages
         * @return True if read was successful, false otherwise
         */
        bool read(std::string message_key, bool search = false);

        /**
         * @brief Whether or not a message has been found
         */
        bool found_;

         /**
         * @brief Wether all blocks from GNSS have arrived for GpsFix Message
         */
        bool gnss_gpsfix_complete(uint32_t id);

        /**
         * @brief Wether all blocks from INS have arrived for GpsFix Message
         */
        bool ins_gpsfix_complete(uint32_t id);

        /**
         * @brief Wether all blocks from GNSS have arrived for NavSatFix Message
         */
        bool gnss_navsatfix_complete(uint32_t id);

        /**
         * @brief Wether all blocks from INS have arrived for NavSatFix Message
         */
        bool ins_navsatfix_complete(uint32_t id);

        /**
         * @brief Wether all blocks from GNSS have arrived for Pose Message
         */
        bool gnss_pose_complete(uint32_t id);

        /**
         * @brief Wether all blocks from INS have arrived for Pose Message
         */
        bool ins_pose_complete(uint32_t id);

        /**
         * @brief Wether all blocks have arrived for Diagnostics Message
         */
        bool diagnostics_complete(uint32_t id);

        /**
         * @brief Wether all blocks have arrived for Imu Message
         */
        bool imu_complete(uint32_t id);

         /**
         * @brief Wether all blocks have arrived for Localization Message
         */
        bool ins_localization_complete(uint32_t id);

    private:
        /**
         * @brief Pointer to the node
         */
        ROSaicNodeBase* node_;

        /**
         * @brief Timestamp of receiving buffer
         */
        Timestamp recvTimestamp_;

        /**
         * @brief Pointer to the buffer of messages
         */
        const uint8_t* data_;

        /**
         * @brief Number of unread bytes in the buffer
         */
        std::size_t count_;

        /**
         * @brief Whether the CRC check as evaluated in the read() method was
         * successful or not is stored here
         */
        bool crc_check_;

        /**
         * @brief Helps to determine size of response message / NMEA message / SBF
         * block
         */
        std::size_t message_size_;

        /**
         * @brief Number of times the GPSFixMsg message has been published
         */
        uint32_t count_gpsfix_ = 0;

        /**
         * @brief Since NavSatFix etc. need PVTGeodetic, incoming PVTGeodetic blocks
         * need to be stored
         */
        PVTGeodetic last_pvtgeodetic_;

        /**
         * @brief Since NavSatFix etc. need PosCovGeodetic, incoming PosCovGeodetic
         * blocks need to be stored
         */
        PosCovGeodetic last_poscovgeodetic_;

        /**
         * @brief Since GPSFix etc. need AttEuler, incoming AttEuler blocks need to
         * be stored
         */
        AttEuler last_atteuler_;

        /**
         * @brief Since GPSFix etc. need AttCovEuler, incoming AttCovEuler blocks
         * need to be stored
         */
        AttCovEuler last_attcoveuler_;

        /**
         * @brief Since NavSatFix, GPSFix, Imu and Pose. need INSNavGeod, incoming INSNavGeod blocks
         * need to be stored
         */
        INSNavGeodMsg last_insnavgeod_;

         /**
         * @brief Since Imu needs ExtSensorMeas, incoming ExtSensorMeas blocks
         * need to be stored
         */
        ExtSensorMeas last_extsensmeas_;

        /**
         * @brief Since GPSFix needs ChannelStatus, incoming ChannelStatus blocks
         * need to be stored
         */
        ChannelStatus last_channelstatus_;

        /**
         * @brief Since GPSFix needs MeasEpoch (for SNRs), incoming MeasEpoch blocks
         * need to be stored
         */
        MeasEpoch last_measepoch_;

        /**
         * @brief Since GPSFix needs DOP, incoming DOP blocks need to be stored
         */
        DOP last_dop_;

        /**
         * @brief Since GPSFix needs VelCovGeodetic, incoming VelCovGeodetic blocks
         * need to be stored
         */
        VelCovGeodetic last_velcovgeodetic_;

        /**
         * @brief Since DiagnosticArray needs ReceiverStatus, incoming ReceiverStatus
         * blocks need to be stored
         */
        ReceiverStatus last_receiverstatus_;

        /**
         * @brief Since DiagnosticArray needs QualityInd, incoming QualityInd blocks
         * need to be stored
         */
        QualityInd last_qualityind_;

        /**
         * @brief Since DiagnosticArray needs ReceiverSetup, incoming ReceiverSetup
         * blocks need to be stored
         */
        ReceiverSetup last_receiversetup_;

        //! Shorthand for the map responsible for matching PVTGeodetic's Mode field
        //! to an enum value
        typedef std::map<uint16_t, TypeOfPVT_Enum> TypeOfPVTMap;

        /**
         * @brief All instances of the RxMessage class shall have access to the map
         * without reinitializing it, hence static
         */
        TypeOfPVTMap type_of_pvt_map;

        //! Shorthand for the map responsible for matching ROS message identifiers to
        //! an enum value
        typedef std::map<std::string, RxID_Enum> RxIDMap;

        /**
         * @brief All instances of the RxMessage class shall have access to the map
         * without reinitializing it, hence static
         *
         * This map is for mapping ROS message, SBF and NMEA identifiers to an
         * enumeration and makes the switch-case formalism in rx_message.hpp more
         * explicit.
         */
        RxIDMap rx_id_map;

        //! When reading from an SBF file, the ROS publishing frequency is governed by the
        //! time stamps found in the SBF blocks therein.
        Timestamp unix_time_;

        //! For GPSFix: Whether the ChannelStatus block of the current epoch has arrived or
        //! not
        bool channelstatus_has_arrived_gpsfix_ = false;

        //! For GPSFix: Whether the MeasEpoch block of the current epoch has arrived or not
        bool measepoch_has_arrived_gpsfix_ = false;

        //! For GPSFix: Whether the DOP block of the current epoch has arrived or not
        bool dop_has_arrived_gpsfix_ = false;

        //! For GPSFix: Whether the PVTGeodetic block of the current epoch has arrived or not
        bool pvtgeodetic_has_arrived_gpsfix_ = false;

        //! For GPSFix: Whether the PosCovGeodetic block of the current epoch has arrived or
        //! not
        bool poscovgeodetic_has_arrived_gpsfix_ = false;

        //! For GPSFix: Whether the VelCovGeodetic block of the current epoch has arrived or
        //! not
        bool velcovgeodetic_has_arrived_gpsfix_ = false;

        //! For GPSFix: Whether the AttEuler block of the current epoch has arrived or not
        bool atteuler_has_arrived_gpsfix_ = false;

        //! For GPSFix: Whether the AttCovEuler block of the current epoch has arrived or not
        bool attcoveuler_has_arrived_gpsfix_ = false;

        //! For GPSFix: Whether the INSNavGeod block of the current epoch has arrived or not
        bool insnavgeod_has_arrived_gpsfix_ = false;

        //! For NavSatFix: Whether the PVTGeodetic block of the current epoch has arrived or
        //! not
        bool pvtgeodetic_has_arrived_navsatfix_ = false;

        //! For NavSatFix: Whether the PosCovGeodetic block of the current epoch has arrived
        //! or not
        bool poscovgeodetic_has_arrived_navsatfix_ = false;

        //! For NavSatFix: Whether the INSNavGeod block of the current epoch has arrived
        //! or not
        bool insnavgeod_has_arrived_navsatfix_ = false;
        //! For PoseWithCovarianceStamped: Whether the PVTGeodetic block of the current epoch
        //! has arrived or not
        bool pvtgeodetic_has_arrived_pose_ = false;

        //! For PoseWithCovarianceStamped: Whether the PosCovGeodetic block of the current
        //! epoch has arrived or not
        bool poscovgeodetic_has_arrived_pose_ = false;

        //! For PoseWithCovarianceStamped: Whether the AttEuler block of the current epoch
        //! has arrived or not
        bool atteuler_has_arrived_pose_ = false;

        //! For PoseWithCovarianceStamped: Whether the AttCovEuler block of the current epoch
        //! has arrived or not
        bool attcoveuler_has_arrived_pose_ = false;

        //! For PoseWithCovarianceStamped: Whether the INSNavGeod block of the current epoch
        //! has arrived or not
        bool insnavgeod_has_arrived_pose_ = false;

        //! For DiagnosticArray: Whether the ReceiverStatus block of the current epoch has
        //! arrived or not
        bool receiverstatus_has_arrived_diagnostics_ = false;
        
        //! For DiagnosticArray: Whether the QualityInd block of the current epoch has
        //! arrived or not
        bool qualityind_has_arrived_diagnostics_ = false;

        //! For Imu: Whether the INSNavGeod block of the current epoch
        //! has arrived or not
        uint8_t insnavgeod_has_arrived_imu_ = 0;

        //! For Localization: Whether the INSNavGeod block of the current epoch
        //! has arrived or not
        bool insnavgeod_has_arrived_localization_ = false;

        //! For Imu: Whether the ExtSensorMeas block of the current epoch
        //! has arrived or not
        bool extsens_has_arrived_imu_ = false;

        /**
         * @brief Callback function when reading PVTCartesian blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message PVTCartesian
         * @return A smart pointer to the ROS message PVTCartesian just created
         */
        PVTCartesianMsgPtr
        PVTCartesianCallback(PVTCartesian& data);

        /**
         * @brief Callback function when reading PVTGeodetic blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message PVTGeodetic
         * @return A smart pointer to the ROS message PVTGeodetic just created
         */
        PVTGeodeticMsgPtr
        PVTGeodeticCallback(PVTGeodetic& data);

        /**
         * @brief Callback function when reading PosCovCartesian blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message PosCovCartesian
         * @return A smart pointer to the ROS message PosCovCartesian just created
         */
        PosCovCartesianMsgPtr
        PosCovCartesianCallback(PosCovCartesian& data);

        /**
         * @brief Callback function when reading PosCovGeodetic blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message PosCovGeodetic
         * @return A smart pointer to the ROS message PosCovGeodetic just created
         */
        PosCovGeodeticMsgPtr
        PosCovGeodeticCallback(PosCovGeodetic& data);

        /**
         * @brief Callback function when reading VelCovGeodetic blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message VelCovGeodetic
         * @return A smart pointer to the ROS message VelCovGeodetic just created
         */
        VelCovGeodeticMsgPtr
        VelCovGeodeticCallback(VelCovGeodetic& data);

        /**
         * @brief Callback function when reading AttEuler blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message AttEuler
         * @return A smart pointer to the ROS message AttEuler just created
         */
        AttEulerMsgPtr AttEulerCallback(AttEuler& data);

        /**
         * @brief Callback function when reading AttCovEuler blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message AttCovEuler
         * @return A smart pointer to the ROS message AttCovEuler just created
         */
        AttCovEulerMsgPtr
        AttCovEulerCallback(AttCovEuler& data);

        /**
         * @brief Callback function when reading INSNavCart blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message INSNavCart
         * @return A smart pointer to the ROS message INSNavCart just created
         */
        INSNavCartMsgPtr
        INSNavCartCallback(INSNavCart& data);

        /**
         * @brief Callback function when reading IMUSetup blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message IMUSetup
         * @return A smart pointer to the ROS message IMUSetup just created
         */
        IMUSetupMsgPtr
        IMUSetupCallback(IMUSetup& data);

        /**
         * @brief Callback function when reading VelSensorSetup blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message VelSensorSetup
         * @return A smart pointer to the ROS message VelSensorSetup just created
         */
        VelSensorSetupMsgPtr
        VelSensorSetupCallback(VelSensorSetup& data);

        /**
         * @brief Callback function when reading ExtSensorMeas blocks
         * @param[in] data The (packed and aligned) struct instance used to populate
         * the ROS message ExtSensorMeas
         * @return A smart pointer to the ROS message ExtSensorMeas just created
         */
        ExtSensorMeasMsgPtr
        ExtSensorMeasCallback(ExtSensorMeas& data);

        /**
         * @brief "Callback" function when constructing NavSatFix messages
         * @return A smart pointer to the ROS message NavSatFix just created
         */
        NavSatFixMsgPtr NavSatFixCallback();

        /**
         * @brief "Callback" function when constructing GPSFix messages
         * @return A smart pointer to the ROS message GPSFix just created
         */
        GPSFixMsgPtr GPSFixCallback();

        /**
         * @brief "Callback" function when constructing PoseWithCovarianceStamped
         * messages
         * @return A smart pointer to the ROS message PoseWithCovarianceStamped just
         * created
         */
        PoseWithCovarianceStampedMsgPtr
        PoseWithCovarianceStampedCallback();

        /**
         * @brief "Callback" function when constructing
         * DiagnosticArrayMsg messages
         * @return A smart pointer to the ROS message
         * DiagnosticArrayMsg just created
         */
        DiagnosticArrayMsgPtr DiagnosticArrayCallback();

        /**
         * @brief "Callback" function when constructing
         * ImuMsg messages
         * @return A smart pointer to the ROS message
         * ImuMsg just created
         */
        ImuMsgPtr ImuCallback();

        /**
         * @brief "Callback" function when constructing
         * LocalizationUtmMsg messages
         * @return A smart pointer to the ROS message
         * LocalizationUtmMsg just created
         */
        LocalizationUtmMsgPtr LocalizationUtmCallback();

         /**
         * @brief Waits according to time when reading from file
         */
        void wait(Timestamp time_obj);

        /**
         * @brief Wether all elements are true
         */
        bool allTrue(std::vector<bool>& vec, uint32_t id);  

        /**
         * @brief Settings struct
         */
        Settings* settings_;

        /**
         * @brief Fixed UTM zone
         */
        std::shared_ptr<std::string> fixedUtmZone_;

        /**
         * @brief Calculates the timestamp, in the Unix Epoch time format
         * This is either done using the TOW as transmitted with the SBF block (if
         * "use_gnss" is true), or using the current time.
         * @param[in] tow (Time of Week) Number of milliseconds that elapsed since the
         * beginning of the current GPS week as transmitted by the SBF block
         * @param[in] wnc (Week Number Counter) counts the number of complete weeks 
         * elapsed since January 6, 1980
         * @param[in] use_gnss If true, the TOW as transmitted with the SBF block is
         * used, otherwise the current time
         * @return Timestamp object containing seconds and nanoseconds since last epoch
         */
        Timestamp timestampSBF(uint32_t tow, uint16_t wnc, bool use_gnss_time);    
    };
} // namespace io_comm_rx
#endif // for RX_MESSAGE_HPP