# import the necessary packages
from imutils.video import VideoStream
from flask import Response
from flask import Flask
from flask import request
from flask import render_template
import threading
import argparse
import datetime
import imutils
import time
import cv2

# initialize the output frame and a lock used to ensure thread-safe
# exchanges of the output frames (useful when multiple browsers/tabs
# are viewing the stream)
outputFrame = None
lock = threading.Lock()

# initialize a flask object
app = Flask(__name__)

# initialize the video stream and allow the camera sensor to
# warmup
#vs = VideoStream(usePiCamera=1).start()
vs = VideoStream(src=0).start()
time.sleep(2.0)
start = time.time()
fps = 0
fps_limit = 30
resize = 100
frame_counter = 0

@app.route("/")
def index():
	# return the rendered template
	return render_template("index.html")

def frame_rate():
	# calculate time since last time the function was called
	global outputFrame, start, fps, resize
	end = time.time()
	delay = end - start
	fps_inst = 1 / delay
	fps = fps * 0.8 + fps_inst * 0.2
	cv2.putText(outputFrame, str(int(fps)), (int(outputFrame.shape[1] * 0.8), 20),
		cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1)
	cv2.putText(outputFrame, str(resize), (10, 20),
		cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1)
	start = time.time()

def process_frame():
	global vs, outputFrame, lock, fps, resize, frame_counter

	# loop over frames from the video stream
	while True:
		# read the next frame from the video stream, resize it,
		frame = vs.read()
		if frame is not None:
			if fps < fps_limit:
				resize -= 5
			else:
				resize += 5
			if resize < 100:
				resize = 100
			if resize > 480:
				resize = 480

			frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
			frame = imutils.resize(frame, width=resize, inter=cv2.INTER_NEAREST)

			# acquire lock, set the output frame, release lock
			with lock:
				frame_counter += 1
				outputFrame = frame.copy()
				frame = None
			time.sleep(0)

def generate_colour():
	global vs, resize, frame_counter

	while False:
		if frame_counter > 10:
			# read the next frame from the video stream, resize it,
			colour_frame = vs.read()
			frame_counter = 0

			if colour_frame is not None:
				colour_frame = imutils.resize(colour_frame, width=40, inter=cv2.INTER_NEAREST)

				# encode the frame in JPEG format
				#colour_encoded = cv2.imencode(".png", colour_frame)
				cv2.imwrite('colour.png', colour_frame)
				colour_encoded = open('colour.png', 'rb').read()

				# yield the output frame in the byte format
				yield(b'--frame\r\n' b'Content-Type: image/png\r\n\r\n' +
					bytearray(colour_encoded) + b'\r\n')

def generate():
	global outputFrame, lock, delay

	# loop over frames from the output stream
	while True:
		# wait until the lock is acquired
		with lock:
			# check if the output frame is available, otherwise skip
			# the iteration of the loop
			if outputFrame is None:
				continue

			frame_rate()
			# encode the frame in JPEG format at 45% quality
			(flag, encodedImage) = cv2.imencode(".jpg", outputFrame,
				[int(cv2.IMWRITE_JPEG_QUALITY), 45])

			# ensure the frame was successfully encoded
			if not flag:
				continue
		time.sleep(0)

		# yield the output frame in the byte format
		yield(b'--frame\r\n' b'Content-Type: image/jpg\r\n\r\n' +
			bytearray(encodedImage) + b'\r\n')

@app.route("/colour")
def colour():
	# return a colour image with transparency
	return Response(generate_colour(),
		mimetype = "multipart/x-mixed-replace; boundary=frame")

@app.route("/video_feed")
def video_feed():
	# return the response generated along with the specific media
	# type (mime type)
	return Response(generate(),
		mimetype = "multipart/x-mixed-replace; boundary=frame")

@app.route("/getTime", methods=['GET'])
def getTime():
    print("browser time: ", request.args.get("time"))
    print("server time : ", time.strftime('%A %B, %d %Y %H:%M:%S'));
    return "Done"

# check to see if this is the main thread of execution
if __name__ == '__main__':

	# construct the argument parser and parse command line arguments
	ap = argparse.ArgumentParser()
	ap.add_argument("-i", "--ip", type=str, default="0.0.0.0",
		help="ip address of the device")
	ap.add_argument("-o", "--port", type=int, default=8000,
		help="ephemeral port number of the server (1024 to 65535)")
	args = vars(ap.parse_args())

	# start a thread to process frames in the background
	t = threading.Thread(target=process_frame)
	t.daemon = True
	t.start()

	# start the flask app
	app.run(host=args["ip"], port=args["port"], debug=True,
		threaded=True, use_reloader=False)

# release the video stream pointer
vs.stop()
