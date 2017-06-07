$('#on-button').click(function() {
    $.ajax({
        type: 'POST',
        url: 'http://localhost:1337/on'
    });
});

$('#off-button').click(function() {
    $.ajax({
        type: 'POST',
        url: 'http://localhost:1337/off'
    });
});

$('#connect1-button').click(function() {
    $.ajax({
        type: 'POST',
        url: 'http://localhost:1337/connect1'
    });
});

$('#connect2-button').click(function() {
    $.ajax({
        type: 'POST',
        url: 'http://localhost:1337/connect2'
    });
});

$('#connect3-button').click(function() {
    $.ajax({
        type: 'POST',
        url: 'http://localhost:1337/connect3'
    });
});

$('#start-button').click(function() {
    $.ajax({
        type: 'POST',
        url: 'http://localhost:1337/start'
    });
});

$('#stop-button').click(function() {
    $.ajax({
        type: 'POST',
        url: 'http://localhost:1337/stop'
    });
});

$('#disconnect-button').click(function() {
    $.ajax({
        type: 'POST',
        url: 'http://localhost:1337/disconnect'
    });
});
