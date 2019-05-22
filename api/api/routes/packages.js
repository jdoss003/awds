const express = require('express');
const mongoose = require('mongoose');
const router = express.Router();

//mongoose package schema
const Package = require('../models/package');

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

router.get('/:packageID', (req, res, next) => {
    const id = req.params.packageID;
    Package.find({packageID: id})
    .exec()
    .then(doc => {
        res.send(doc);
    })
    .catch(err => {
        res.send(err);
    })
});

router.post('/', (req, res, next) => {
    const package = new Package({
        packageID: req.body.packageID,
        details: req.body.details
    });

    package.save().then(result => {
        res.send(result);
        })
        .catch(err => res.send(err));

    res.status(201).json({
        message: 'Handling package POST requests',
        Package: package
    });
});

router.delete('/:packageID', (req, res, next) =>{
    const id = req.params.packageID;
    Package.remove({packageID: id})
    .exec()
    .then(doc => {
        res.send(doc);
    })
    .catch(err => {
        res.send(err);
    })
})

module.exports = router;