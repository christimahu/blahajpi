/**
 * Blahaj PI Website JavaScript
 * Handles animations and footer behavior
 */
(function() {
    'use strict';

    // Wait for DOM to be fully loaded
    document.addEventListener('DOMContentLoaded', () => {
        // Elements
        const logo = document.querySelector('.logo');
        const sharkEmoji = document.querySelector('.emoji-shark');
        const magnifyEmoji = document.querySelector('.emoji-magnify');
        const docButton = document.querySelector('.docs-button i');
        const gitButton = document.querySelector('.github-button i');
        const footer = document.getElementById('sliding-footer');
                
        // Animation sequence timing
        const animationTimings = {
            shark: 500,       // Shark animation starts after 0.5s
            magnify: 1500,    // Magnifying glass after 1.5s
            docs: 3500,       // Docs button after 3.5s (2s after magnify)
            github: 4500      // GitHub button after 4.5s (1s after docs)
        };
        
        // Sequence animations
        if (sharkEmoji) {
            sharkEmoji.style.animation = `jiggle 0.5s ease ${animationTimings.shark}ms`;
        }
        
        if (magnifyEmoji) {
            magnifyEmoji.style.animation = `jiggle 0.5s ease ${animationTimings.magnify}ms`;
        }
        
        // Use setTimeout for button animations
        setTimeout(() => {
            if (docButton) docButton.classList.add('wiggle');
        }, animationTimings.docs);
        
        setTimeout(() => {
            if (gitButton) gitButton.classList.add('wiggle');
        }, animationTimings.github);        
    });
})();
