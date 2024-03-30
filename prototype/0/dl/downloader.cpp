#include "downloader.h"

using namespace dl;
using namespace curlpp::options;

/**
 * Initialise the object by setting the progress callback function.
 * \param l pointer to a listener object
 */
Downloader::Downloader(DownloadListener* l) : prog_func_(this,&Downloader::progress_callback), downloading_(false), dlsize_(0.0), dlcur_(0.0), listener_(l) {
}

/**
 * Set the listener.
 * \param l pointer to a listener object
 */
void Downloader::set_listener(DownloadListener* l) {
	listener_=l;
}

/**
 * Set the base URL from which to download.
 * \param url URL from which to download
 */
void Downloader::set_url(const std::string& url) {
	url_=url;
}

/**
 * Start downloading a particular file, and write the data to a file.
 * \param name name of file to download
 * \param filename file to which data will be written
 */
void Downloader::start_download(const std::string& name, const std::string& filename) {
	if(downloading_) return;
	output_.open(filename.c_str(),std::fstream::out);
	if(!output_.is_open()) return;
	request_.setOpt(new Url(url_+name));
	request_.setOpt(new NoProgress(0));
	request_.setOpt(ProgressFunction(prog_func_));
	request_.setOpt(WriteStream(&output_));
	multi_.add(&request_);
	downloading_=true;
}

/**
 * Reset members after a download is complete.
 */
void Downloader::finish_download() {
	downloading_=false;
	multi_.remove(&request_);
	request_.reset();
	output_.close();
}

/**
 * Abort the current download.
 */
void Downloader::abort_download() {
	if(!downloading_) return;
	finish_download();
}

/**
 * Get the value of downloading_.
 * \return whether a file is currently downloading
 */
bool Downloader::downloading() {
	return downloading_;
}

/**
 * Update the current download.
 * \returns whether or not the file is still downloading
 */
bool Downloader::update() {
	if(!downloading_) return false;
	int handles;
	multi_.perform(&handles);
	if(handles==0) finish_download();
	return downloading_;
}

/**
 * Get the total size of the file being downloaded.
 * \returns size (in bytes) of file
 */
double Downloader::dlsize() {
	return (downloading_?dlsize_:0.01); // to avoid division by 0
}

/**
 * Get the number of bytes currently downloaded.
 * \returns number of bytes currently downloaded
 */
double Downloader::dlcur() {
	return (downloading_?dlcur_:0.0);
}

/**
 * Progress has been made on the current download, so tell the listener (if it exists).
 * \param dltot total size (in bytes) of file being downloaded
 * \param dlnow current number of bytes downloaded
 */
double Downloader::progress_callback(double dltot, double dlnow, double,double) {
	dlsize_=dltot;
	dlcur_=dlnow;
	if(listener_) listener_->on_progress(dlnow, dltot);
	return 0.0;
}
