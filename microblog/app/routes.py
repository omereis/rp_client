from flask import Flask, render_template, request
from app import app

from flask import render_template
from app import app

import socket

g_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

@app.route('/')
@app.route('/index')
def index():
    user = {'username': 'Joe'}
    posts = [
        {
            'date' : {'Jan. 17, 2020'},
            'author': {'username': 'John'},
            'body': 'Beautiful day in Portland: firey but mostly peacefull'
        },
        {
            'date' : {'Jan. 18, 2021'},
            'author': {'username': 'Susan'},
            'body': 'The Avengers movie was so cool!'
        }
    ]
    return render_template('index.html', title='Home', user=user, posts=posts)

@app.route('/onconnect', methods=["GET"])
def on_connect ():
    print ('Just clicked')
    res = {}
    try:
        res = request.args['message']
#        ws = websocket.create_connection(f'ws://{bumps_params.server}:{bumps_params.mp_port}')
        print (f'on_connect message:\n{res}')
    except Exception as e:
        print (f'run time error in OnConnect\n: {e}')
    return ('on_connect result')


 