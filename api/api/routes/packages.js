const express = require('express');
const mongoose = require('mongoose');
const router = express.Router();

//mongoose package schema
const Package = require('../models/package');

//Create package
router.post('/', (req, res, next) => {
    const package = new Package({
        PACKAGE_ID: req.body.PACKAGE_ID,
        DETAILS: req.body.DETAILS,
        GCODE: req.body.GCODE
    });

    package.save().then(result => {
        res.send(result);
        })
        .catch(err => res.send(err));

    res.status(201).json({
        Package: package
    });
});

//Delete package
router.delete('/:packageID', (req, res, next) =>{
    const id = req.params.packageID;
    Package.remove({packageID: id})
    .exec()
    .then(doc => {
        res.send(doc);
    })
    .catch(err => {
        res.send(err);
    });
});

//Get all packages
router.get('/', (req, res, next) => {
    Package.find()
    .exec()
    .then(docs => {
        res.send(docs);
    })
    .catch(err => {
        res.send(err);
    });
});

//Get package by ID
router.get('/:packageID', (req, res, next) => {
    const id = req.params.packageID;
    Package.find({packageID: id})
    .exec()
    .then(doc => {
        res.send(doc);
    })
    .catch(err => {
        res.send(err);
    });
});

module.exports = router;
