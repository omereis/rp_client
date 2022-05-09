from flask import Flask,render_template
app = Flask(__name__)

@app.route("/")
def index():
    return render_template ("rp_gui.html")
   
@app.route("/red_pitaya_setup")
def red_pitaya_setup():
    read_card_setup()
    return render_template ("rp_setup.html")
   
   
@app.route("/other_links")
def other_links():
    return render_template ("rp_setup.html")

if (__name__ == '__main__'):
    app.run (host='0.0.0.0', port=5005, debug=True)
