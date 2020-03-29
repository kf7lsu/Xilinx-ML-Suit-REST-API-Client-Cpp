from __future__ import print_function

import argparse
import json
import time

import caffe
import flask
import numpy as np
import xdnn_io

app = flask.Flask(__name__)

net = None
tranformer = None


class Job:
    def __init__(self):
        self.img = None
        self.profile = {}

    def start_timer(self):
        self.profile["start_time"] = time.time()

    def record_time(self, name):
        self.profile[name] = time.time() - self.profile["start_time"]

    def print_profile(self):
        records = [(key, value) for (key, value) in self.profile.items()]
        records.sort(key=lambda x: x[1])
        for key, value in records:
            print(key, ":", value)


def LoadImage(prototxt, caffemodel, labels):
    global net
    global transformer
    net = caffe.Net(prototxt, caffemodel, caffe.TEST)
    print(net.blobs['data'].data.shape)

    # Setup preprocessor
    transformer = caffe.io.Transformer({'data': net.blobs['data'].data.shape})
    transformer.set_transpose('data', (2, 0, 1))
    # transformer.set_mean('data', np.array([104,117,123]))
    # transformer.set_raw_scale('data', 255)
    transformer.set_channel_swap('data', (2, 1, 0))  # if using RGB instead if BGR


def InferImage(net, image, labels):
    global transformer
    net.blobs['data'].data[...] = transformer.preprocess('data', image)
    out = net.forward()
    softmax = None
    for key in out:
        try:
            if out[key].shape[1] == 1000:
                softmax = out[key]
        except:
            pass
    # Labels = xdnn_io.get_labels(labels)
    # xdnn_io.printClassification(softmax, [image], Labels)
    return None  # [x for x in softmax]  # xdnn_io.getClassification(softmax, [image], Labels)


@app.route("/predict", methods=["POST"])
def predict():
    job = Job()
    job.start_timer()

    data = {"success": False}

    global prototxt
    global model
    global synset_words

    if flask.request.method == "POST":

        job.record_time("start")

        # Get image
        images = flask.request.form["image"]

        job.record_time("get_image")

        # Decode array
        images = json.loads(images)

        job.record_time("decode_image")

        # Inference
        images = np.array(images, dtype=np.float32)
        images = images.reshape((-1, 224, 224, 3))

        job.record_time("convert_to_np_array")

        responses = []
        splitted_images = [images[i] for i in range(images.shape[0])]

        job.record_time("split_image")

        for img in splitted_images:
            responses.append(InferImage(net, img, synset_words))

        job.record_time("inference")

        # Write response
        data["success"] = True
        data["response"] = responses

        job.record_time("save_result")

    result = flask.jsonify(data)

    job.record_time("encode_result")
    job.print_profile()

    return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='pyXFDNN')
    parser.add_argument('--caffemodel', default="/opt/models/caffe/bvlc_googlenet/bvlc_googlenet.caffemodel",
                        help='path to caffemodel file eg: /opt/models/caffe/bvlc_googlenet/bvlc_googlenet.caffemodel')
    parser.add_argument('--prototxt', default="xfdnn_auto_cut_deploy.prototxt",
                        help='path to  prototxt file eg: xfdnn_auto_cut_deploy.prototxt')
    parser.add_argument('--synset_words', default="$HOME/CK-TOOLS/dataset-imagenet-ilsvrc2012-aux/synset_words.txt",
                        help='path to synset_words eg: $HOME/CK-TOOLS/dataset-imagenet-ilsvrc2012-aux/synset_words.txt')
    parser.add_argument('--port', default=5000)

    args = vars(parser.parse_args())

    if args["caffemodel"]:
        model = args["caffemodel"]

    if args["prototxt"]:
        prototxt = args["prototxt"]

    if args["synset_words"]:
        synset_words = args["synset_words"]

    if args["port"]:
        port = args["port"]

    print("Loading FPGA with image...")
    LoadImage(prototxt, model, synset_words)

    print("Starting Flask Server...")
    app.run('0.0.0.0', port=port)
