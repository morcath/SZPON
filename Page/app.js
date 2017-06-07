var express = require('express');
var cmd=require('./js/cmd.js');
var app = express();

app.post('/on', function(req, res) {
    console.log('ON');
    // Run your LED toggling code here
	cmd.get(
	    `
		gnome-terminal -x sh -c \"tmux new-session -s MYSES; exec bash\"
		tmux send-keys -t MYSES "../System/system2"\'\n\'
	    `,
	    function(err,data){
		console.log(data)
	    }
	);
});

app.post('/off', function(req, res) {
    console.log('OFF');
    // Run your LED toggling code here
	cmd.get(
	    `
		tmux send-keys -t MYSES "0"\'\n\'
		tmux send-keys -t MYSES "exit"\'\n\'		
		tmux send-keys -t MYSES "exit"\'\n\'
	    `,
	    function(err,data){
		console.log(data)
	    }
	);
});

app.post('/connect1', function(req, res) {
    console.log('CONNECT1');
    // Run your LED toggling code here
	cmd.get(
	    `
		tmux send-keys -t MYSES "1"\'\n\'
		tmux send-keys -t MYSES "1"\'\n\'		
	    `,
	    function(err,data){
		console.log(data)
	    }
	);
});

app.post('/connect2', function(req, res) {
    console.log('CONNECT2');
    // Run your LED toggling code here
	cmd.get(
	    `
		tmux send-keys -t MYSES "1"\'\n\'
		tmux send-keys -t MYSES "2"\'\n\'		
	    `,
	    function(err,data){
		console.log(data)
	    }
	);
});

app.post('/connect3', function(req, res) {
    console.log('CONNECT3');
    // Run your LED toggling code here
	cmd.get(
	    `
		tmux send-keys -t MYSES "1"\'\n\'
		tmux send-keys -t MYSES "3"\'\n\'		
	    `,
	    function(err,data){
		console.log(data)
	    }
	);
});

app.post('/start', function(req, res) {
    console.log('START');
    // Run your LED toggling code here
	cmd.get(
	    `
		tmux send-keys -t MYSES "2"\'\n\'
		tmux send-keys -t MYSES "1"\'\n\'
		tmux send-keys -t MYSES "5 50"\'\n\'		
	    `,
	    function(err,data){
		console.log(data)
	    }
	);
});

app.post('/stop', function(req, res) {
    console.log('STOP');
    // Run your LED toggling code here
	cmd.get(
	    `
		tmux send-keys -t MYSES "2"\'\n\'
		tmux send-keys -t MYSES "2"\'\n\'		
	    `,
	    function(err,data){
		console.log(data)
	    }
	);
});

app.post('/disconnect', function(req, res) {
    console.log('DISCONNECT');
    // Run your LED toggling code here
	cmd.get(
	    `
		tmux send-keys -t MYSES "3"\'\n\'		
	    `,
	    function(err,data){
		console.log(data)
	    }
	);
});

app.listen(1337);
