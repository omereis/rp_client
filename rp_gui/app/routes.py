from flask import Flask, render_template, request
from app import app
import json

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
    return render_template('index.html', title='Home', user=user, posts=posts, status='disconnected')

@app.route('/onconnect', methods=["GET"])
def on_connect ():
    print ('Just clicked')
    res = {}
    strReply=""
    try:
        res = request.args['message']
        print(f'res: "{res}"')
        print(f'Type(res): {type(res)}')
        dictMessage = json.loads(res)
        print(f'Dictionary message: "{dictMessage}"')
        print(f'Host: {dictMessage["host"]}')
        print(f'Port: {dictMessage["port"]}')
        g_socket.connect((dictMessage["host"],dictMessage["port"]))
        strReply ="connection ok"
        g_socket.close()
#        ws = websocket.create_connection(f'ws://{bumps_params.server}:{bumps_params.mp_port}')
        print (f'on_connect message:\n{res}')
    except Exception as e:
        strReply ="connection ok"
        print (f'run time error in OnConnect\n: {e}')
    return (strReply)
#    return ('on_connect result')


 