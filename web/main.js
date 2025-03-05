/**
 * Blahaj PI Coming Soon Page
 * Main JavaScript functionality
 */
(function() {
    'use strict';
    
    // Initialize when DOM is fully loaded
    document.addEventListener('DOMContentLoaded', init);
    
    /**
     * Initialize all page functionality
     */
    function init() {
        const elements = {
            video: document.querySelector('video'),
            introVideo: document.getElementById('intro-video'),
            mainContent: document.getElementById('main-content'),
            sharkEmoji: document.querySelector('.emoji-shark'),
            magnifyEmoji: document.querySelector('.emoji-magnify'),
            githubIcon: document.querySelector('.github-link i'),
            body: document.body
        };
        
        // Start with initial black screen, then fade in to first frame of video
        setTimeout(() => {
            // Fade in the video (initially hidden)
            elements.video.classList.add('visible');
            elements.introVideo.classList.add('fade-in-video');
            
            // Start playing after video has faded in halfway
            setTimeout(() => {
                startVideo(elements);
            }, 500);
        }, 500);
    }
    
    /**
     * Start playing the video
     * @param {Object} elements - DOM elements
     */
    function startVideo(elements) {
        const { video, introVideo } = elements;
        
        // Set up video events before playing
        setupVideoEvents(elements);
        
        // Play the video
        video.play().catch(e => console.log('Autoplay prevented. User interaction required.'));
    }
    
    /**
     * Set up all video-related event listeners
     * @param {Object} elements - DOM elements
     */
    function setupVideoEvents(elements) {
        const { video, introVideo } = elements;
        
        // Trigger when video ends naturally
        video.addEventListener('ended', () => {
            // Wait half a second after the movie is done before fading out
            setTimeout(() => {
                fadeOutVideo(elements);
            }, 500);
        });
        
        // Fix for Safari and some mobile browsers
        video.addEventListener('canplaythrough', () => {
            // Video is ready to play
        });
    }
    
    /**
     * Fade out video and transition to main content
     * @param {Object} elements - DOM elements
     */
    function fadeOutVideo(elements) {
        const { introVideo, mainContent, body } = elements;
        
        // Simple fade out
        introVideo.classList.add('video-fading');
        
        // Show main content as video fades out
        setTimeout(() => {
            mainContent.style.display = 'block';
            body.classList.add('show-background');
            
            // Hide video container after fade completes
            setTimeout(() => {
                introVideo.style.display = 'none';
                
                // Start animation sequence
                startAnimationSequence(elements);
            }, 500);
        }, 750);
    }
    
    /**
     * Start the sequence of animations
     * @param {Object} elements - DOM elements
     */
    function startAnimationSequence(elements) {
        const { sharkEmoji, magnifyEmoji, githubIcon } = elements;
        
        // First animation - jiggle shark after 4s
        setTimeout(() => {
            animateElement(sharkEmoji, 'jiggle', 500);
            
            // Second animation - jiggle magnifying glass after 4s more
            setTimeout(() => {
                animateElement(magnifyEmoji, 'jiggle', 500);
                
                // Third animation - jiggle GitHub icon after 4s more
                setTimeout(() => {
                    animateElement(githubIcon, 'jiggle', 500);
                }, 4000);
            }, 4000);
        }, 4000);
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
