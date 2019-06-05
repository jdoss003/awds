const express = require('express');
const mongoose = require('mongoose');
const router = express.Router();

//mongoose package schema
const Package = require('../models/package');

//Create package
router.post('/', (req, res, next) => {

    var gcode = [];
    switch (req.body.CUBBY_NUM)
    {
        case 1: gcode.push("G1 X154 Y167");
            break;
        case 2: gcode.push("G1 X462 Y507");
            break;
        case 3: gcode.push("G1 X462 Y167");
            break;
        case 4: gcode.push("G1 X720 Y502");
            break;
        case 5: gcode.push("G1 X720 Y167");
            break;
        default:
            break;
    }
    gcode.push("G52");
    gcode.push("G1 Z143");
    gcode.push("G50");
    gcode.push("G1 Z0");
    gcode.push("G1 X154 Y502");
    gcode.push("G1 Z143");
    gcode.push("G50");
    gcode.push("G1 Z0");

    const package = new Package({
        PACKAGE_ID: req.body.PACKAGE_ID,
        DETAILS: req.body.DETAILS,
        GCODE: gcode
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
router.delete('/:packageId', (req, res, next) =>{
    const id = req.params.packageId;
    Package.remove({_id: id})
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
    Package.find({PACKAGE_ID: id})
    .exec()
    .then(doc => {
        res.send(doc);
    })
    .catch(err => {
        res.send(err);
    });
});

module.exports = router;
