const express = require('express');
const app = express();
const morgan = require('morgan');
const bodyparser = require('body-parser');
const mongoose = require('mongoose');

//Define routes
const packageRoutes = require('./api/routes/packages');
const operationRoutes = require('./api/routes/operations');

//Connect to database
mongoose.connect('mongodb://localhost/awds', {useNewUrlParser: true})

//log requests using morgan
app.use(morgan('dev'));

//Parse any body string given
app.use(bodyparser.urlencoded({extended: false}));
app.use(bodyparser.json());

//Routes
app.use('/packages', packageRoutes);
app.use('/operations', operationRoutes);

//CORS handling
app.use((req, res, next) => {
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow_Headers', 'Origin, X-Requested-With, Content-Type, Accept, Authorization');
    if(req.method == 'OPTIONS') {
        res.header('Access-Control-Allow-Methods', 'PUT, POST, PATCH, DELETE, GET')
        return res.status.json({});
    }
});

//Handle routes
app.use((req, res, next) => {
    const err = new Error('Not Found');
    err.status = 404;
    next(err);
});

//Handle any error thrown
app.use((err, req, res, next) => {
    res.status(err.status || 500);
    res.json({
        error: {
            message: err.message
        }
    });
});

module.exports = app;