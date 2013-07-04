/**************************************************************************
 *  Copyright 2012 KISS Institute for Practical Robotics                  *
 *                                                                        *
 *  This file is part of libkovan.                                        *
 *                                                                        *
 *  libkovan is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  libkovan is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with libkovan. Check the LICENSE file in the project root.      *
 *  If not, see <http://www.gnu.org/licenses/>.                           *
 **************************************************************************/

#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include "geom.hpp"
#include "color.hpp"
#include "config.hpp"
 #include "export.h"
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sys/time.h>

#include <opencv2/core/core.hpp>

// These keys are used in the config files loaded by
// Camera::Device
#define CAMERA_GROUP ("camera")
#define CAMERA_NUM_CHANNELS_KEY ("num_channels")
#define CAMERA_CHANNEL_GROUP_PREFIX ("channel_")
#define CAMERA_CHANNEL_TYPE_KEY ("type")

#define CAMERA_CHANNEL_TYPE_HSV_KEY ("hsv")
#define CAMERA_CHANNEL_TYPE_QR_KEY ("qr")

namespace cv
{
	class VideoCapture;
}

namespace Camera
{
	class Device;
	
	class EXPORT_SYM Object
	{
	public:
		Object(const Point2<unsigned> &centroid,
			const Rect<unsigned> &boundingBox,
			const double &confidence, const char *data = 0,
			const size_t &dataLength = 0);
		Object(const Object &rhs);
		~Object();
		
		const Point2<unsigned> &centroid() const;
		const Rect<unsigned> &boundingBox() const;
		const double &confidence() const;
		const char *data() const;
		const size_t &dataLength() const;
		
	private:
		Point2<unsigned> m_centroid;
		Rect<unsigned> m_boundingBox;
		double m_confidence;
		char *m_data;
		size_t m_dataLength;
	};
	
	typedef std::vector<Object> ObjectVector;
	
	class EXPORT_SYM ChannelImpl
	{
	public:
		ChannelImpl();
		virtual ~ChannelImpl();
		
		void setImage(const cv::Mat &image);
		ObjectVector objects(const Config &config);
		
	protected:
		virtual void update(const cv::Mat &image) = 0;
		virtual ObjectVector findObjects(const Config &config) = 0;
		
	private:
		bool m_dirty;
		cv::Mat m_image;
	};
	
	class EXPORT_SYM ChannelImplManager
	{
	public:
		virtual ~ChannelImplManager();
		virtual void setImage(const cv::Mat &image) = 0;
		virtual ChannelImpl *channelImpl(const std::string &name) = 0;
	};
	
	class EXPORT_SYM DefaultChannelImplManager : public ChannelImplManager
	{
	public:
		DefaultChannelImplManager();
		~DefaultChannelImplManager();
		
		virtual void setImage(const cv::Mat &image);
		virtual ChannelImpl *channelImpl(const std::string &name);
		
	private:
		std::map<std::string, ChannelImpl *> m_channelImpls;
	};
	
	class EXPORT_SYM Channel
	{
	public:
		Channel(Device *device, const Config &config);
		~Channel();
		
		void invalidate();
		
		const ObjectVector *objects() const;
		
		Device *device() const;
		
		/**
		 * Do not call this method unless you know what you are doing!
		 */ 
		void setConfig(const Config &config);
		
	private:
		Device *m_device;
		Config m_config;
		mutable ObjectVector m_objects;
		ChannelImpl *m_impl;
		mutable bool m_valid;
	};
	
	typedef std::vector<Channel *> ChannelPtrVector;
	
	class EXPORT_SYM ConfigPath
	{
	public:
		static std::string extension();
		
		static void setBasePath(const std::string &path);
		static std::string path(const std::string &name = std::string());
		static std::string defaultPath();
		static std::string defaultConfigPath();
		static void setDefaultConfigPath(const std::string &name);
		
	private:
		static std::string s_path;
	};
	
	class EXPORT_SYM Device
	{
	public:
		Device();
		~Device();
		
		bool open(const int &number = 0);
		bool isOpen() const;
		void close();
		bool update();
		
		void setWidth(const unsigned &width);
		void setHeight(const unsigned &height);
		void setGrabCount(unsigned char grabs);
		unsigned char grabCount() const;
		
		const ChannelPtrVector &channels() const;
		
		cv::VideoCapture *videoCapture() const;
		const cv::Mat &rawImage() const;
		
		void setConfig(const Config &config);
		const Config &config() const;
		
		void setChannelImplManager(ChannelImplManager *channelImplManager);
		ChannelImplManager *channelImplManager() const;
		
	private:
		void updateConfig();
		
		Config m_config;
		cv::VideoCapture *m_capture;
		ChannelPtrVector m_channels;
		ChannelImplManager *m_channelImplManager;
		unsigned char m_grabCount;
		cv::Mat m_image;
		timeval m_lastUpdate;
	};
}



#endif
