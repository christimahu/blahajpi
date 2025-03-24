/**
 * Mobile detection for Blahaj PI documentation
 */
(function() {
  'use strict';
  
  document.addEventListener('DOMContentLoaded', function() {
    // Simple mobile detection using both media query and user agent
    const isMobile = window.matchMedia('(max-width: 768px)').matches || 
                     /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
    
    if (isMobile) {
      // Show mobile message
      const mobileMessage = document.getElementById('mobile-message');
      if (mobileMessage) {
        mobileMessage.classList.remove('hidden');
      }
      
      // Hide documentation content
      const mainContent = document.getElementById('doc-content');
      const sideNav = document.getElementById('side-nav');
      
      if (mainContent) mainContent.classList.add('hidden');
      if (sideNav) sideNav.classList.add('hidden');
      
      // Try to directly target the hamburger menu
      hideAllNavigationElements();
      
      // Add event listener to the "Continue Anyway" button
      const continueButton = document.getElementById('continue-anyway');
      if (continueButton) {
        continueButton.addEventListener('click', function() {
          // Hide mobile message
          if (mobileMessage) mobileMessage.classList.add('hidden');
          
          // Show documentation content
          if (mainContent) mainContent.classList.remove('hidden');
          if (sideNav) sideNav.classList.remove('hidden');
        });
      }
    }
  });
  
  /**
   * Hide all navigation and search elements on mobile using multiple approaches
   */
  function hideAllNavigationElements() {
    // Search elements
    const elementsToHide = [
      'MSearchBox',
      'MSearchField',
      'MSearchSelect',
      'FSearchBox',
      'searchli',
      'main-menu',
      'main-nav',
      'nav-tree',
      'nav-sync',
      'nav-path',
      'menu-icon',
      'menu-trigger'
    ];
    
    // Hide elements by ID
    elementsToHide.forEach(function(id) {
      const element = document.getElementById(id);
      if (element) {
        element.style.display = 'none';
        element.style.visibility = 'hidden';
      }
    });
    
    // Also hide elements by class
    const classesToHide = [
      'tabs',
      'tabs2',
      'tabs3',
      'navpath',
      'ui-resizable-handle',
      'hamburger-menu',
      'sm-dox',
      'mobile-nav',
      'menu-item',
      'menu-toggle',
      'dropdown-menu',
      'menu-button',
      'menu-trigger',
      'navigation-controls'
    ];
    
    classesToHide.forEach(function(className) {
      const elements = document.getElementsByClassName(className);
      for (let i = 0; i < elements.length; i++) {
        elements[i].style.display = 'none';
        elements[i].style.visibility = 'hidden';
      }
    });
    
    // Direct access to elements using more aggressive selectors
    document.querySelectorAll('button, a.hamburger-menu, .hamburger-menu, svg[viewBox="0 0 30 30"]').forEach(function(element) {
      if (element.textContent === '☰' || element.innerHTML.includes('☰') || element.classList.contains('hamburger')) {
        element.style.display = 'none';
        element.style.visibility = 'hidden';
      }
    });
  }
})();
