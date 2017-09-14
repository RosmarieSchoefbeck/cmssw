/*!
  \file TrackerALignmentErrorExtended_PayloadInspector
  \Payload Inspector Plugin for Tracker Alignment Errors (APE)
  \author M. Musich
  \version $Revision: 1.0 $
  \date $Date: 2017/07/10 10:59:24 $
*/

#include "CondCore/Utilities/interface/PayloadInspectorModule.h"
#include "CondCore/Utilities/interface/PayloadInspector.h"
#include "CondCore/CondDB/interface/Time.h"

// the data format of the condition to be inspected
#include "CondFormats/Alignment/interface/AlignmentErrorsExtended.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h" 

// needed for the tracker map
#include "CommonTools/TrackerMap/interface/TrackerMap.h"

#include <memory>
#include <sstream>

const float cmToUm = 10000;

namespace {

  /************************************************
    1d histogram of sqrt(d_xx) of 1 IOV 
  *************************************************/

  // inherit from one of the predefined plot class: Histogram1D
  class TrackerAlignmentErrorExtendedXValue : public cond::payloadInspector::Histogram1D<AlignmentErrorsExtended> {
    
  public:
    TrackerAlignmentErrorExtendedXValue () : cond::payloadInspector::Histogram1D<AlignmentErrorsExtended>("TrackerAlignmentErrorExtendedXValue",
													  "TrackerAlignmentErrorExtendedXValue",500,0.0,500.0){
      Base::setSingleIov( true );
    }
    
    bool fill( const std::vector<std::tuple<cond::Time_t,cond::Hash> >& iovs ){
      for ( auto const & iov: iovs) {
	std::shared_ptr<AlignmentErrorsExtended> payload = Base::fetchPayload( std::get<1>(iov) );
	if( payload.get() ){
	  
	  std::vector<AlignTransformErrorExtended> alignErrors = payload->m_alignError;
	  for (const auto& it : alignErrors ){

	    CLHEP::HepSymMatrix errMatrix = it.matrix();
	
	    
	    int subid = DetId(it.rawId()).subdetId();	    

	    if(subid==1){
	      std::cout << it.rawId() << " | " << sqrt(errMatrix[0][0])*cmToUm << std::endl;
	    }

	    // to be used to fill the histogram
	    fillWithValue(sqrt(errMatrix[0][0])*cmToUm);
	    
	  } // loop on the vector of modules
	}// payload
      }// iovs
      return true;
    }// fill
  };


  /************************************************
    1d histogram of sqrt(d_yy) of 1 IOV 
  *************************************************/

  // inherit from one of the predefined plot class: Histogram1D
  class TrackerAlignmentErrorExtendedYValue : public cond::payloadInspector::Histogram1D<AlignmentErrorsExtended> {
    
  public:
    TrackerAlignmentErrorExtendedYValue () : cond::payloadInspector::Histogram1D<AlignmentErrorsExtended>("TrackerAlignmentErrorExtendedYValue",
													  "TrackerAlignmentErrorExtendedYValue",500,0.0,500.0){
      Base::setSingleIov( true );
    }
    
    bool fill( const std::vector<std::tuple<cond::Time_t,cond::Hash> >& iovs ){
      for ( auto const & iov: iovs) {
	std::shared_ptr<AlignmentErrorsExtended> payload = Base::fetchPayload( std::get<1>(iov) );
	if( payload.get() ){
	  
	  std::vector<AlignTransformErrorExtended> alignErrors = payload->m_alignError;
	  for (const auto& it : alignErrors ){

	    CLHEP::HepSymMatrix errMatrix = it.matrix();
		   
	    // to be used to fill the histogram
	    fillWithValue(sqrt(errMatrix[1][1])*cmToUm);
	    
	  } // loop on the vector of modules
	}// payload
      }// iovs
      return true;
    }// fill
  };

  /************************************************
    1d histogram of sqrt(d_zz) of 1 IOV 
  *************************************************/

  // inherit from one of the predefined plot class: Histogram1D
  class TrackerAlignmentErrorExtendedZValue : public cond::payloadInspector::Histogram1D<AlignmentErrorsExtended> {
    
  public:
    TrackerAlignmentErrorExtendedZValue () : cond::payloadInspector::Histogram1D<AlignmentErrorsExtended>("TrackerAlignmentErrorExtendedZValue",
													  "TrackerAlignmentErrorExtendedZValue",500,0.0,500.0){
      Base::setSingleIov( true );
    }
    
    bool fill( const std::vector<std::tuple<cond::Time_t,cond::Hash> >& iovs ){
      for ( auto const & iov: iovs) {
	std::shared_ptr<AlignmentErrorsExtended> payload = Base::fetchPayload( std::get<1>(iov) );
	if( payload.get() ){
	  
	  std::vector<AlignTransformErrorExtended> alignErrors = payload->m_alignError;
	  for (const auto& it : alignErrors ){

	    CLHEP::HepSymMatrix errMatrix = it.matrix();

	    // to be used to fill the histogram
	    fillWithValue(sqrt(errMatrix[2][2])*cmToUm);
	    
	  } // loop on the vector of modules
	}// payload
      }// iovs
      return true;
    }// fill
  };

  // /************************************************
  //   TrackerMap of sqrt(d_xx)
  // *************************************************/
  class TrackerAlignmentErrorExtendedXTrackerMap : public cond::payloadInspector::PlotImage<AlignmentErrorsExtended> {
  public:
    TrackerAlignmentErrorExtendedXTrackerMap() : cond::payloadInspector::PlotImage<AlignmentErrorsExtended>( "Tracker Map of d_{xx} of APE" ){
      setSingleIov( true );
    }

    bool fill( const std::vector<std::tuple<cond::Time_t,cond::Hash> >& iovs ){
      auto iov = iovs.front();
      std::shared_ptr<AlignmentErrorsExtended> payload = fetchPayload( std::get<1>(iov) );

      std::string titleMap = "APE d_{xx} value (payload : "+std::get<1>(iov)+")";

      std::unique_ptr<TrackerMap> tmap = std::unique_ptr<TrackerMap>(new TrackerMap("APE_dxx"));
      tmap->setTitle(titleMap.c_str());
      tmap->setPalette(1);
   
      std::vector<AlignTransformErrorExtended> alignErrors = payload->m_alignError;

      bool isPhase0(false);
      if(alignErrors.size()==19876) isPhase0 = true;
      
      for (const auto& it : alignErrors ){

	CLHEP::HepSymMatrix errMatrix = it.matrix();

	// fill the tracker map

	int subid = DetId(it.rawId()).subdetId();	    

	if(isPhase0){
	  tmap->addPixel(true);
	  tmap->fill(it.rawId(),sqrt(errMatrix[0][0])*cmToUm);
	} else {
	  if(subid!=1 && subid!=2){
	    tmap->fill(it.rawId(),sqrt(errMatrix[0][0])*cmToUm);
	  }
	}
      } // loop over detIds

      //=========================
   
      std::string fileName(m_imageFileName);
      tmap->save(true,0,0,fileName.c_str());

      return true;
    }
  };

  // /************************************************
  //   TrackerMap of SiStripApvGains (average gain per detid)
  // *************************************************/
  // class SiStripApvGainsAverageTrackerMap : public cond::payloadInspector::PlotImage<SiStripApvGain> {
  // public:
  //   SiStripApvGainsAverageTrackerMap() : cond::payloadInspector::PlotImage<SiStripApvGain>( "Tracker Map of average SiStripGains" ){
  //     setSingleIov( true );
  //   }

  //   bool fill( const std::vector<std::tuple<cond::Time_t,cond::Hash> >& iovs ){
  //     auto iov = iovs.front();
  //     std::shared_ptr<SiStripApvGain> payload = fetchPayload( std::get<1>(iov) );

  //     std::string titleMap = "SiStrip APV Gain average per module (payload : "+std::get<1>(iov)+")";

  //     std::unique_ptr<TrackerMap> tmap = std::unique_ptr<TrackerMap>(new TrackerMap("SiStripApvGains"));
  //     tmap->setTitle(titleMap.c_str());
  //     tmap->setPalette(1);
      
  //     std::vector<uint32_t> detid;
  //     payload->getDetIds(detid);
      
  //     for (const auto & d : detid) {
  // 	SiStripApvGain::Range range=payload->getRange(d);
  // 	float sumOfGains=0;
  // 	float nAPVsPerModule=0.;
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPVsPerModule+=1;
  // 	  sumOfGains+=payload->getApvGain(it,range);
  // 	} // loop over APVs
  // 	// fill the tracker map taking the average gain on a single DetId
  // 	tmap->fill(d,(sumOfGains/nAPVsPerModule));
  //     } // loop over detIds

  //     //=========================
      
  //     std::string fileName(m_imageFileName);
  //     tmap->save(true,0,0,fileName.c_str());

  //     return true;
  //   }
  // };
  
  // /************************************************
  //   TrackerMap of SiStripApvGains (ratio with previous gain per detid)
  // *************************************************/
  // class SiStripApvGainsRatioWithPreviousIOVTrackerMap : public cond::payloadInspector::PlotImage<SiStripApvGain> {
  // public:
  //   SiStripApvGainsRatioWithPreviousIOVTrackerMap() : cond::payloadInspector::PlotImage<SiStripApvGain>( "Tracker Map of ratio of SiStripGains with previous IOV" ){
  //     setSingleIov( false );
  //   }

  //   bool fill( const std::vector<std::tuple<cond::Time_t,cond::Hash> >& iovs ){
      
  //     std::vector<std::tuple<cond::Time_t,cond::Hash> > sorted_iovs = iovs;

  //     // make absolute sure the IOVs are sortd by since
  //     std::sort(begin(sorted_iovs), end(sorted_iovs), [](auto const &t1, auto const &t2) {
  // 	  return std::get<0>(t1) < std::get<0>(t2);
  // 	});
      
  //     auto firstiov  = sorted_iovs.front();
  //     auto lastiov   = sorted_iovs.back();
      
  //     std::shared_ptr<SiStripApvGain> last_payload  = fetchPayload( std::get<1>(lastiov) );
  //     std::shared_ptr<SiStripApvGain> first_payload = fetchPayload( std::get<1>(firstiov) );
      
  //     std::string titleMap = "SiStrip APV Gain ratio per module average (IOV: ";

  //     titleMap+=std::to_string(std::get<0>(firstiov));
  //     titleMap+="/ IOV:";
  //     titleMap+=std::to_string(std::get<0>(lastiov));
  //     titleMap+=")";

  //     std::unique_ptr<TrackerMap> tmap = std::unique_ptr<TrackerMap>(new TrackerMap("SiStripApvGains"));
  //     tmap->setTitle(titleMap.c_str());
  //     tmap->setPalette(1);

  //     std::map<uint32_t,float> lastmap,firstmap;

  //     std::vector<uint32_t> detid;
  //     last_payload->getDetIds(detid);

  //     // cache the last IOV
  //     for (const auto & d : detid) {
  // 	SiStripApvGain::Range range=last_payload->getRange(d);
  // 	float Gain=0;
  // 	float nAPV=0;
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPV+=1;
  // 	  Gain+=last_payload->getApvGain(it,range);
  // 	} // loop over APVs
  // 	lastmap[d]=(Gain/nAPV);
  //     } // loop over detIds
      
  //     detid.clear();
      
  //     first_payload->getDetIds(detid);
      
  //     // cache the first IOV
  //     for (const auto & d : detid) {
  // 	SiStripApvGain::Range range=first_payload->getRange(d);
  // 	float Gain=0;
  // 	float nAPV=0;
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPV+=1;
  // 	  Gain+=first_payload->getApvGain(it,range);
  // 	} // loop over APVs
  // 	firstmap[d]=(Gain/nAPV);
  //     } // loop over detIds
      

  //     std::map<uint32_t,float> cachedRatio; 
  //     for(const auto &d : detid){
  // 	float ratio = firstmap[d]/lastmap[d];
  // 	tmap->fill(d,ratio);
  // 	cachedRatio[d] = ratio;
  //     }
    
  //     //=========================
  //     auto range = getTheRange(cachedRatio);

  //     std::string fileName(m_imageFileName);
  //     tmap->save(true,range.first,range.second,fileName.c_str());

  //     return true;
  //   }
  // };

  // /************************************************
  //   TrackerMap of SiStripApvGains (ratio for largest deviation with previous gain per detid)
  // *************************************************/
  // class SiStripApvGainsRatioMaxDeviationWithPreviousIOVTrackerMap : public cond::payloadInspector::PlotImage<SiStripApvGain> {
  // public:
  //   SiStripApvGainsRatioMaxDeviationWithPreviousIOVTrackerMap() : cond::payloadInspector::PlotImage<SiStripApvGain>( "Tracker Map of ratio (for largest deviation) of SiStripGains with previous IOV" ){
  //     setSingleIov( false );
  //   }

  //   bool fill( const std::vector<std::tuple<cond::Time_t,cond::Hash> >& iovs ){
      
  //     std::vector<std::tuple<cond::Time_t,cond::Hash> > sorted_iovs = iovs;

  //     // make absolute sure the IOVs are sortd by since
  //     std::sort(begin(sorted_iovs), end(sorted_iovs), [](auto const &t1, auto const &t2) {
  // 	  return std::get<0>(t1) < std::get<0>(t2);
  // 	});
      
  //     auto firstiov  = sorted_iovs.front();
  //     auto lastiov   = sorted_iovs.back();
      
  //     std::shared_ptr<SiStripApvGain> last_payload  = fetchPayload( std::get<1>(lastiov) );
  //     std::shared_ptr<SiStripApvGain> first_payload = fetchPayload( std::get<1>(firstiov) );
      
  //     std::string titleMap = "SiStrip APV Gain ratio for largest deviation per module (IOV: ";

  //     titleMap+=std::to_string(std::get<0>(firstiov));
  //     titleMap+="/ IOV:";
  //     titleMap+=std::to_string(std::get<0>(lastiov));
  //     titleMap+=")";

  //     std::unique_ptr<TrackerMap> tmap = std::unique_ptr<TrackerMap>(new TrackerMap("SiStripApvGains"));
  //     tmap->setTitle(titleMap.c_str());
  //     tmap->setPalette(1);

  //     std::map<std::pair<uint32_t,int>,float> lastmap,firstmap;

  //     std::vector<uint32_t> detid;
  //     last_payload->getDetIds(detid);
      
  //     // cache the last IOV
  //     for (const auto & d : detid) {
  // 	SiStripApvGain::Range range=last_payload->getRange(d);
  // 	float Gain=0;
  // 	float nAPV=0;
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPV+=1;
  // 	  Gain+=last_payload->getApvGain(it,range);
  // 	  std::pair<uint32_t,int> index = std::make_pair(d,nAPV);
  // 	  lastmap[index]=(Gain/nAPV);
  // 	} // loop over APVs
  //     } // loop over detIds
      
  //     detid.clear();
      
  //     first_payload->getDetIds(detid);
      
  //     // cache the first IOV
  //     for (const auto & d : detid) {
  // 	SiStripApvGain::Range range=first_payload->getRange(d);
  // 	float Gain=0;
  // 	float nAPV=0;
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPV+=1;
  // 	  Gain+=first_payload->getApvGain(it,range);
  // 	  std::pair<uint32_t,int> index = std::make_pair(d,nAPV);
  // 	  firstmap[index]=(Gain/nAPV);
  // 	} // loop over APVs
  //     } // loop over detIds
      
  //     // find the largest deviation
  //     std::map<uint32_t,float> cachedRatio; 

  //     for(const auto &item : firstmap ){
	
  // 	// packed index (detid,APV)
  // 	auto index   = item.first;
  // 	auto mod     = item.first.first;
	
  // 	float ratio = firstmap[index]/lastmap[index];
  // 	// if we have already cached something
  // 	if(cachedRatio[mod]){
  // 	  if(std::abs(cachedRatio[mod])>std::abs(ratio)){
  // 	    cachedRatio[mod]=ratio;
  // 	  }
  // 	} else {
  // 	  cachedRatio[mod]=ratio;
  // 	}
  //     }

  //     for (const auto &element : cachedRatio){
  // 	tmap->fill(element.first,element.second);
  //     }

  //     // get the range of the TrackerMap (saturate at +/-2 std deviations)
  //     auto range = getTheRange(cachedRatio);
      
  //     //=========================
      
  //     std::string fileName(m_imageFileName);
  //     tmap->save(true,range.first,range.second,fileName.c_str());

  //     return true;
  //   }
  // };

  // /************************************************
  //   TrackerMap of SiStripApvGains (maximum gain per detid)
  // *************************************************/
  // class SiStripApvGainsMaximumTrackerMap : public cond::payloadInspector::PlotImage<SiStripApvGain> {
  // public:
  //   SiStripApvGainsMaximumTrackerMap() : cond::payloadInspector::PlotImage<SiStripApvGain>( "Tracker Map of SiStripAPVGains (maximum per DetId)" ){
  //     setSingleIov( true );
  //   }
    
  //   bool fill( const std::vector<std::tuple<cond::Time_t,cond::Hash> >& iovs ){
  //     auto iov = iovs.front();
  //     std::shared_ptr<SiStripApvGain> payload = fetchPayload( std::get<1>(iov) );

  //     std::string titleMap = "SiStrip APV Gain maximum per module (payload : "+std::get<1>(iov)+")";

  //     std::unique_ptr<TrackerMap> tmap = std::unique_ptr<TrackerMap>(new TrackerMap("SiStripApvGains"));
  //     tmap->setTitle(titleMap.c_str());
  //     tmap->setPalette(1);
      
  //     std::vector<uint32_t> detid;
  //     payload->getDetIds(detid);
      
  //     for (const auto & d : detid) {
  // 	SiStripApvGain::Range range=payload->getRange(d);
  // 	float theMaxGain=0;
  // 	for(int it=0;it<range.second-range.first;it++){
	  
  // 	  float currentGain = payload->getApvGain(it,range);
  // 	  if(currentGain > theMaxGain){
  // 	    theMaxGain=currentGain;
  // 	  }
  // 	} // loop over APVs
  // 	// fill the tracker map taking the average gain on a single DetId
  // 	tmap->fill(d,theMaxGain);
  //     } // loop over detIds

  //     //=========================
      
  //     std::string fileName(m_imageFileName);
  //     tmap->save(true,0,0,fileName.c_str());

  //     return true;
  //   }
  // };

  // /************************************************
  //   TrackerMap of SiStripApvGains (minimum gain per detid)
  // *************************************************/
  // class SiStripApvGainsMinimumTrackerMap : public cond::payloadInspector::PlotImage<SiStripApvGain> {
  // public:
  //   SiStripApvGainsMinimumTrackerMap() : cond::payloadInspector::PlotImage<SiStripApvGain>( "Tracker Map of SiStripAPVGains (minimum per DetId)" ){
  //     setSingleIov( true );
  //   }

  //   bool fill( const std::vector<std::tuple<cond::Time_t,cond::Hash> >& iovs ){
  //     auto iov = iovs.front();
  //     std::shared_ptr<SiStripApvGain> payload = fetchPayload( std::get<1>(iov) );

  //     std::string titleMap = "SiStrip APV Gain minumum per module (payload : "+std::get<1>(iov)+")";

  //     std::unique_ptr<TrackerMap> tmap = std::unique_ptr<TrackerMap>(new TrackerMap("SiStripApvGains"));
  //     tmap->setTitle(titleMap.c_str());
  //     tmap->setPalette(1);
      
  //     std::vector<uint32_t> detid;
  //     payload->getDetIds(detid);
      
  //     for (const auto & d : detid) {
  // 	SiStripApvGain::Range range=payload->getRange(d);
  // 	float theMinGain=999.;
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  float currentGain = payload->getApvGain(it,range);
  // 	  if(currentGain < theMinGain){
  // 	    theMinGain=currentGain;
  // 	  }
  // 	} // loop over APVs
  // 	// fill the tracker map taking the average gain on a single DetId
  // 	tmap->fill(d,theMinGain);
  //     } // loop over detIds

  //     //=========================
      
  //     std::string fileName(m_imageFileName);
  //     tmap->save(true,0,0,fileName.c_str());

  //     return true;
  //   }
  // };


  // /************************************************
  //   time history histogram of SiStripApvGains 
  // *************************************************/

  // class SiStripApvGainByRunMeans : public cond::payloadInspector::HistoryPlot<SiStripApvGain,float> {
  // public:
  //   SiStripApvGainByRunMeans() : cond::payloadInspector::HistoryPlot<SiStripApvGain,float>( "SiStripApv Gains average","average Strip APV gain value"){}
  //   virtual ~SiStripApvGainByRunMeans() = default;

  //   float getFromPayload( SiStripApvGain& payload ){
     
  //     std::vector<uint32_t> detid;
  //     payload.getDetIds(detid);
      
  //     float nAPVs=0;
  //     float sumOfGains=0;

  //     for (const auto & d : detid) {
  // 	SiStripApvGain::Range range=payload.getRange(d);
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPVs+=1;
  // 	  sumOfGains+=payload.getApvGain(it,range);
  // 	} // loop over APVs
  //     } // loop over detIds

  //     return sumOfGains/nAPVs;      
  //   } // payload
  // };

  // /************************************************
  //   time history histogram of TIB SiStripApvGains 
  // *************************************************/

  // class SiStripApvTIBGainByRunMeans : public cond::payloadInspector::HistoryPlot<SiStripApvGain,float> {
  // public:
  //   SiStripApvTIBGainByRunMeans() : cond::payloadInspector::HistoryPlot<SiStripApvGain,float>( "SiStripApv Gains average","average Tracker Inner Barrel APV gain value"){}
  //   virtual ~SiStripApvTIBGainByRunMeans() = default;

  //   float getFromPayload( SiStripApvGain& payload ){
     
  //     std::vector<uint32_t> detid;
  //     payload.getDetIds(detid);
      
  //     float nAPVs=0;
  //     float sumOfGains=0;

  //     for (const auto & d : detid) {

  // 	int subid = DetId(d).subdetId();
  // 	if(subid!=StripSubdetector::TIB) continue;
	
  // 	SiStripApvGain::Range range=payload.getRange(d);
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPVs+=1;
  // 	  sumOfGains+=payload.getApvGain(it,range);
  // 	} // loop over APVs
  //     } // loop over detIds

  //     return sumOfGains/nAPVs;

  //   } // payload
  // };

  // /************************************************
  //   time history histogram of TOB SiStripApvGains 
  // *************************************************/

  // class SiStripApvTOBGainByRunMeans : public cond::payloadInspector::HistoryPlot<SiStripApvGain,float> {
  // public:
  //   SiStripApvTOBGainByRunMeans() : cond::payloadInspector::HistoryPlot<SiStripApvGain,float>( "SiStripApv Gains average","average Tracker Outer Barrel gain value"){}
  //   virtual ~SiStripApvTOBGainByRunMeans() = default;

  //   float getFromPayload( SiStripApvGain& payload ){
     
  //     std::vector<uint32_t> detid;
  //     payload.getDetIds(detid);
      
  //     float nAPVs=0;
  //     float sumOfGains=0;
      
  //     for (const auto & d : detid) {

  // 	int subid = DetId(d).subdetId();
  // 	if(subid!=StripSubdetector::TOB) continue;

  // 	SiStripApvGain::Range range=payload.getRange(d);
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPVs+=1;
  // 	  sumOfGains+=payload.getApvGain(it,range);
  // 	} // loop over APVs
  //     } // loop over detIds

  //     return sumOfGains/nAPVs;

  //   } // payload
  // };

  // /************************************************
  //   time history histogram of TID SiStripApvGains 
  // *************************************************/

  // class SiStripApvTIDGainByRunMeans : public cond::payloadInspector::HistoryPlot<SiStripApvGain,float> {
  // public:
  //   SiStripApvTIDGainByRunMeans() : cond::payloadInspector::HistoryPlot<SiStripApvGain,float>( "SiStripApv Gains average","average Tracker Inner Disks APV gain value"){}
  //   virtual ~SiStripApvTIDGainByRunMeans() = default;

  //   float getFromPayload( SiStripApvGain& payload ){
     
  //     std::vector<uint32_t> detid;
  //     payload.getDetIds(detid);
      
  //     float nAPVs=0;
  //     float sumOfGains=0;
  //     for (const auto & d : detid) {
	
  // 	int subid = DetId(d).subdetId();
  // 	if(subid!=StripSubdetector::TID) continue;
	
  // 	SiStripApvGain::Range range=payload.getRange(d);
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPVs+=1;
  // 	  sumOfGains+=payload.getApvGain(it,range);
  // 	} // loop over APVs
  //     } // loop over detIds

  //     return sumOfGains/nAPVs;

  //   } // payload
  // };

  // /************************************************
  //   time history histogram of TEC SiStripApvGains 
  // *************************************************/

  // class SiStripApvTECGainByRunMeans : public cond::payloadInspector::HistoryPlot<SiStripApvGain,float> {
  // public:
  //   SiStripApvTECGainByRunMeans() : cond::payloadInspector::HistoryPlot<SiStripApvGain,float>( "SiStripApv Gains average in TEC","average Tracker Endcaps APV gain value"){}
  //   virtual ~SiStripApvTECGainByRunMeans() = default;

  //   float getFromPayload( SiStripApvGain& payload ){
     
  //     std::vector<uint32_t> detid;
  //     payload.getDetIds(detid);
      
  //     float nAPVs=0;
  //     float sumOfGains=0;

  //     for (const auto & d : detid) {

  // 	int subid = DetId(d).subdetId();
  // 	if(subid!=StripSubdetector::TEC) continue;
	
  // 	SiStripApvGain::Range range=payload.getRange(d);
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  nAPVs+=1;
  // 	  sumOfGains+=payload.getApvGain(it,range);
  // 	} // loop over APVs
  //     } // loop over detIds

  //     return sumOfGains/nAPVs;

  //   } // payload
  // };

    
} // close namespace

// Register the classes as boost python plugin
PAYLOAD_INSPECTOR_MODULE(TrackerAlignmentErrorExtended){
  PAYLOAD_INSPECTOR_CLASS(TrackerAlignmentErrorExtendedXValue);
  PAYLOAD_INSPECTOR_CLASS(TrackerAlignmentErrorExtendedYValue);
  PAYLOAD_INSPECTOR_CLASS(TrackerAlignmentErrorExtendedZValue);
  PAYLOAD_INSPECTOR_CLASS(TrackerAlignmentErrorExtendedXTrackerMap);
}
