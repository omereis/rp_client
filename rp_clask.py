from flask import Flask
app = Flask(__name__)

@app.route("/")
def index():
    return render_template ("rp_gui.html")
   
if (__name__ == '__main_'):
    flask.run (host='0.0.0.0', port=5000, debug=True)