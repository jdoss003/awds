const mongoose = require('mongoose');

const Package = mongoose.Schema({
    PACKAGE_ID: {
        type: String,
        unique: true,
        required: true,
    },
    DATE_ENTERED: {
        type: Date,
        default: Date.now,
    },
    DETAILS: {
        type: String,
        required: true,
    },
    GCODE: {
        type: String,
        required: true,
    }
});

module.exports = mongoose.model('Package', Package);