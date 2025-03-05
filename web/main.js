/**
 * Blahaj PI Coming Soon Page
 * Main JavaScript functionality
 */
(function() {
    'use strict';
    
    // Initialize when DOM is fully loaded
    document.addEventListener('DOMContentLoaded', init);
    
    /**
     * Check if the device is mobile
     * @return {boolean} True if the device is mobile
     */
    function isMobileDevice() {
        return (
            navigator.userAgent.match(/Android/i) ||
            navigator.userAgent.match(/webOS/i) ||
            navigator.userAgent.match(/iPhone/i) ||
            navigator.userAgent.match(/iPad/i) ||
            navigator.userAgent.match(/iPod/i) ||
            navigator.userAgent.match(/BlackBerry/i) ||
            navigator.userAgent.match(/Windows Phone/i)
        );
    }
    
    /**
     * Initialize all page functionality
     */
    function init() {
        const elements = {
            video: document.querySelector('video'),
            videoContainer: document.getElementById('video-container'),
            logoImage: document.getElementById('logo-image'),
            sharkEmoji: document.querySelector('.emoji-shark'),
            magnifyEmoji: document.querySelector('.emoji-magnify'),
            githubIcon: document.querySelector('.github-link i')
        };
        
        // Check if device is mobile
        if (isMobileDevice()) {
            // On mobile, skip video and show logo directly
            elements.videoContainer.style.display = 'none';
            elements.logoImage.style.opacity = '1';
            startAnimationSequence(elements);
            return;
        }
        
        // Set up video events
        setupVideoEvents(elements);
        
        // Error handling
        elements.video.addEventListener('error', () => {
            elements.videoContainer.style.display = 'none';
            elements.logoImage.style.opacity = '1';
            startAnimationSequence(elements);
        });
    }
    
    /**
     * Set up all video-related event listeners
     * @param {Object} elements - DOM elements
     */
    function setupVideoEvents(elements) {
        const { video, videoContainer } = elements;
        
        // When video ends, wait 1 second before transitioning
        video.addEventListener('ended', () => {
            console.log('Video ended');
            setTimeout(() => {
                transitionToLogo(elements);
            }, 1000); // Wait 1 second on the last frame
        });
        
        // Start video
        video.play().catch(e => {
            console.log('Video playback error:', e);
            // Fallback if video fails to play
            videoContainer.style.display = 'none';
            elements.logoImage.style.opacity = '1';
            startAnimationSequence(elements);
        });
    }
    
    /**
     * Transition from video to logo
     * @param {Object} elements - DOM elements
     */
    function transitionToLogo(elements) {
        const { videoContainer, logoImage } = elements;
        
        console.log('Starting transition to logo');
        
        // Step 1: Change to square shape and keep visible
        videoContainer.classList.add('square');
        
        // Step 2: After shape change, fade out
        setTimeout(() => {
            videoContainer.classList.add('fading');
            
            // Step 3: Fade in the logo
            setTimeout(() => {
                logoImage.style.opacity = '1';
                
                // Step 4: Hide video completely after fade
                setTimeout(() => {
                    videoContainer.style.display = 'none';
                    
                    // Start animation sequence
                    startAnimationSequence(elements);
                }, 250);
            }, 250);
        }, 500); // Time for shape transition
    }
    
    /**
     * Start the sequence of animations
     * @param {Object} elements - DOM elements
     */
    function startAnimationSequence(elements) {
        const { sharkEmoji, magnifyEmoji, githubIcon } = elements;
        
        // First animation - jiggle shark after 1s
        setTimeout(() => {
            animateElement(sharkEmoji, 'jiggle', 500);
            
            // Second animation - jiggle magnifying glass after 2s
            setTimeout(() => {
                animateElement(magnifyEmoji, 'jiggle', 500);
                
                // Third animation - fancy github icon animation after 2s
                setTimeout(() => {
                    animateElement(githubIcon, 'github-dance', 1000);
                }, 2000);
            }, 2000);
        }, 1000);
    }
    
    /**
     * Animate an element with a specific animation
     * @param {Element} element - DOM element to animate
     * @param {string} animationName - CSS animation name
     * @param {number} duration - Animation duration in ms
     */
    function animateElement(element, animationName, duration) {
        element.style.animation = `${animationName} ${duration/1000}s ease`;
        
        setTimeout(() => {
            element.style.animation = 'none';
        }, duration);
    }
})();